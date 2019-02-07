/*
 *            Copyright 2009-2018 The VOTCA Development Team
 *                       (http://www.votca.org)
 *
 *      Licensed under the Apache License, Version 2.0 (the "License")
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef __VOTCA_CTP_NEIGHBORLIST_H
#define __VOTCA_CTP_NEIGHBORLIST_H

#include <votca/ctp/logger.h>
#include <votca/tools/globals.h>
#include <votca/ctp/qmcalculator.h>
#include <votca/ctp/qmpair.h>


namespace votca { namespace ctp {

namespace TOOLS = votca::tools;
    
class Neighborlist : public QMCalculator
{

public:

    Neighborlist() { };
   ~Neighborlist() {
       // cleanup the list of superexchange pair types
       for ( std::list<QMNBList::SuperExchangeType*>::iterator it = _superexchange.begin() ; it != _superexchange.end(); it++  ) {
           delete *it;
       }
    };

    string Identify() { return "neighborlist"; }
    
    void Initialize(Property *options);
    bool EvaluateFrame(Topology *top);
    void GenerateFromFile(Topology *top, string filename);

private:

    bool _useConstantCutoff;
    double _constantCutoff;
    std::map< std::string, std::map<std::string, double> > _cutoffs;

    bool _use_active_fragments;
    std::map< std::string, std::map<std::string, std::string> > _active_fragments;
    
    bool _generate_from_file;
    std::string _file_name;

    std::list<QMNBList::SuperExchangeType*> _superexchange;

    Logger _log;

};
    

void Neighborlist::Initialize(Property *options) {

    // update options with the VOTCASHARE defaults   
    UpdateWithDefaults( options,"ctp" );
    std::string key = "options." + Identify();

    // properties of the logger
    _log.setPreface(logINFO,    "\n... ...");
    _log.setPreface(logERROR,   "\n... ...");
    _log.setPreface(logWARNING, "\n... ...");
    _log.setPreface(logDEBUG,   "\n... ...");  
         
    if (TOOLS::globals::verbose) {
        _log.setReportLevel( logDEBUG ); 
    } else {
        _log.setReportLevel( logINFO ); 
    }
    
    list< Property* > segs = options->Select(key+".segments");
    list< Property* > ::iterator segsIt;

    for (segsIt = segs.begin(); segsIt != segs.end(); segsIt++) {
      
        double cutoff = (*segsIt)->get("cutoff").as<double>();

        // get pairs of segment types 
        string types = (*segsIt)->get("type").as<string>();
        
        Tokenizer tok(types, " ");
        vector< string > names;
        tok.ToVector(names);

        if (names.size() != 2) {
            CTP_LOG(logERROR,_log) << "Faulty pair definition for cut-off's: "
                 << "Need two segment names separated by a space" << std::flush;
            throw std::runtime_error("Error in options file.");
        }

        _cutoffs[names[0]][names[1]] = cutoff;
        _cutoffs[names[1]][names[0]] = cutoff;
        
        // get active fragments for each pair of segments
        
        std::string fragments;
        _use_active_fragments = false;
        if ((*segsIt)->exists("fragments")) {
            // if * is provided, do not use fragments filtering
            fragments = (*segsIt)->get("fragments").as<string>();

            std::size_t found = fragments.find("*");
            if (found == std::string::npos) {
                _use_active_fragments = true;
                CTP_LOG(logWARNING,_log) << "Using filtering by fragments: " 
                        << fragments << std::flush; 
            }
            
        } else {
            CTP_LOG(logWARNING,_log) 
                    << "From v1.6 you can use a list of fragments (or '*')." 
                    << std::flush;
        }
        
        _active_fragments[names[0]][names[1]] = fragments;
        _active_fragments[names[1]][names[0]] = fragments;

    }

    if (options->exists(key+".constant")) {
        _useConstantCutoff = true;
        _constantCutoff = options->get(key+".constant").as< double >();
    }
    else {
        _useConstantCutoff = false;
    }

    _generate_from_file = false;    
    if (options->exists(key+".file")) {
        _file_name = options->get(key+".file").as< string >();
        if ( _file_name.size() != 0 ) _generate_from_file = true;
    }

    // if superexchange is given
    if (options->exists(key + ".superexchange")) {
        list< Property* > _se = options->Select(key + ".superexchange");
        list< Property* > ::iterator seIt;

        for (seIt = _se.begin(); seIt != _se.end(); seIt++) {
            string types = (*seIt)->get("type").as<string>();
            QMNBList::SuperExchangeType* _su = new QMNBList::SuperExchangeType(types);
            _superexchange.push_back(_su); 
        }
    }
            
}

bool Neighborlist::EvaluateFrame(Topology *top) {

    top->NBList().Cleanup();

    if (_generate_from_file) { 
        this->GenerateFromFile(top, _file_name); 
    }
    else {        

        vector< Segment* > ::iterator segit1;
        vector< Segment* > ::iterator segit2;
        vector< Fragment* > ::iterator fragit1;
        vector< Fragment* > ::iterator fragit2;

        double cutoff;
        std::string fragment_list;
        
        vec r1;
        vec r2;    
        
        for (segit1 = top->Segments().begin();
                segit1 < top->Segments().end();
                segit1++) {

                Segment *seg1 = *segit1;
                CTP_LOG(logDEBUG,_log) << "NB List Seg " << seg1->getId() << std::flush;

            for (segit2 = segit1 + 1;
                    segit2 < top->Segments().end();
                    segit2++) {

                Segment *seg2 = *segit2;

                if (!_useConstantCutoff) {
                    // Find cut-off
                    try {
                        cutoff = _cutoffs.at(seg1->getName())
                                         .at(seg2->getName());
                        
                        fragment_list = _active_fragments.at(seg1->getName())
                                                         .at(seg2->getName());
                    }
                    catch (out_of_range&) {
                        CTP_LOG(logERROR,_log) << "ERROR: No cut-off specified for segment pair "
                             << seg1->getName() << " | " << seg2->getName() 
                             << ". " << std::flush;
                        throw std::runtime_error("Missing input in options.");
                    }
                }

                else { cutoff = _constantCutoff; }


                bool stopLoop = false;
                for (fragit1 = seg1->Fragments().begin();
                        fragit1 < seg1->Fragments().end();
                        fragit1 ++) {

                    // check if this fragment is active
                    std::string _frag1name = (*fragit1)->getName();
                    std::size_t found1 = fragment_list.find(_frag1name);
                    if ( _use_active_fragments && found1 == std::string::npos) { continue; }
                    
                    if (stopLoop) { break; }

                    for (fragit2 = seg2->Fragments().begin();
                            fragit2 < seg2->Fragments().end();
                            fragit2++) {

                        // check if this fragment is active
                        std::string _frag2name = (*fragit1)->getName();
                        std::size_t found2 = fragment_list.find(_frag2name);
                        if (_use_active_fragments && found2 == std::string::npos) { continue; }

                        r1 = (*fragit1)->getPos();
                        r2 = (*fragit2)->getPos();
                        if( abs( top->PbShortestConnect(r1, r2) ) > cutoff ) {
                            continue;
                        }
                        else {
                            top->NBList().Add(seg1, seg2);
                            stopLoop = true;
                            break;
                        }                

                    } /* exit loop frag2 */
                } /* exit loop frag1 */
            } /* exit loop seg2 */
                
               // break;
        } /* exit loop seg1 */       

    }

    // add superexchange pairs
    top->NBList().setSuperExchangeTypes(_superexchange);
    top->NBList().GenerateSuperExchange();

    // short summary at the end
    std::map<int, int> npairs;
    for ( QMNBList::iterator pit = top->NBList().begin(); pit != top->NBList().end(); pit++ ) {
        QMPair *pair = (*pit);
        npairs[pair->getType()] += 1;
    }
    
    CTP_LOG(logINFO,_log) <<  "Created " << top->NBList().size() << " pairs." << std::flush;
    CTP_LOG(logINFO,_log) <<  "Hopping only pairs: " << npairs[QMPair::Hopping] << std::flush;
    CTP_LOG(logINFO,_log) <<  "Superexchange pairs: " << npairs[QMPair::SuperExchange] << std::flush;
    CTP_LOG(logINFO,_log) <<  "Superexchange and hopping pairs: " << npairs[QMPair::SuperExchangeAndHopping] << std::flush;
      
    // DEBUG output
    if (votca::tools::globals::verbose) {

	Property bridges_summary;
        Property *_bridges = &bridges_summary.add("bridges","");

        CTP_LOG(logDEBUG,_log) << "Bridged Pairs \n [idA:idB] com distance" << std::flush;
        for (QMNBList::iterator ipair = top->NBList().begin(); ipair != top->NBList().end(); ++ipair) {
                QMPair *pair = *ipair;
                Segment* segment1 = pair->Seg1PbCopy();
                Segment* segment2 = pair->Seg2PbCopy();
                
                CTP_LOG(logDEBUG,_log) << " [" << segment1->getId() << ":" << segment2->getId()<< "] " 
                             << pair->Dist()<< " bridges: " 
                             << (pair->getBridgingSegments()).size() 
                             << " type: " 
                             << pair->getType() 
                             << " | " << std::flush;
                
                vector<Segment*> bsegments = pair->getBridgingSegments();
 
                Property *_pair_property = &_bridges->add("pair","");
                                   
                _pair_property->setAttribute("id1", segment1->getId());
                _pair_property->setAttribute("id2", segment2->getId());
                _pair_property->setAttribute("name1", segment1->getName());
                _pair_property->setAttribute("name2", segment2->getName());
                _pair_property->setAttribute("r12", pair->Dist());
                                    
                Property *_bridge_property = &_pair_property->add("bridge","");

                for ( vector<Segment*>::iterator itb = bsegments.begin(); itb != bsegments.end(); itb++ ) {
                    CTP_LOG(logDEBUG,_log) << (*itb)->getId() << " " ;
                    _bridge_property->setAttribute("id", (*itb)->getId());
                }        
                
                CTP_LOG(logDEBUG,_log) << std::flush;
        }
    }

    std::cout << _log;
    return true;        
}

/* input example
 * 1  1 2 DCV DCV  
 * 2  1 3 DCV DCV     
 * 3  2 3 DCV DCV
 */ 
void Neighborlist::GenerateFromFile(Topology *top, string filename) {
    
    std::string line;
    std::ifstream intt;
    intt.open(filename.c_str());
    
    if (intt.is_open() ) {
        while ( intt.good() ) {

            std::getline(intt, line);
            vector<string> split;
            Tokenizer toker(line, " \t");
            toker.ToVector(split);

            if ( !split.size()      ||
                  split[0] == "!"   ||
                  split[0].substr(0,1) == "!" ) { continue; }
            
            int seg1id          = boost::lexical_cast<int>(split[1]);
            int seg2id          = boost::lexical_cast<int>(split[2]);
            
            Segment* seg1 = top->getSegment(seg1id);
            Segment* seg2 = top->getSegment(seg2id);
            
            string seg1name     = boost::lexical_cast<string>(split[3]);
            string seg2name     = boost::lexical_cast<string>(split[4]);
            assert(seg1->getName() == seg1name);
            assert(seg2->getName() == seg2name);

	    (void)top->NBList().Add(seg1,seg2);
            
        } /* Exit loop over lines */
    }
    else { 
        CTP_LOG(logERROR,_log) << "ERROR: No such file " << filename << std::flush;
        throw std::runtime_error("Supply input file."); 
    }    
}



}}

#endif  /* __NEIGHBORLIST2_H */

/*
 *            Copyright 2009-2012 The VOTCA Development Team
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


#include "igwbse.h"

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

#include <votca/ctp/logger.h>
#include <votca/ctp/qmpackagefactory.h>

using boost::format;
using namespace boost::filesystem;
using namespace votca::tools;

namespace ub = boost::numeric::ublas;
    
namespace votca { namespace ctp {
    
// +++++++++++++++++++++++++++++ //
// IGWBSE MEMBER FUNCTIONS         //
// +++++++++++++++++++++++++++++ //

void IGWBSE::Initialize(votca::tools::Property* options ) {

    _energy_difference = 0.0;
    
    
    // tasks to be done by IBSE: dft_input, dft_run, dft_parse, mgbft, bse_coupling
    _do_dft_input = false;
    _do_dft_run   = false;
    _do_dft_parse = false;
    _do_gwbse     = false;
    _do_coupling  = false;
    _do_trim      = false;
    
    _store_orbitals = false;
    _store_overlap = false;
    _store_integrals = false;
    _store_ehint = false;
    
    // update options with the VOTCASHARE defaults   
    UpdateWithDefaults( options );
    ParseOptionsXML( options  );
    
    // register all QM packages (Gaussian, turbomole, etc))
    QMPackageFactory::RegisterAll();

}

void IGWBSE::ParseOptionsXML( votca::tools::Property *opt ) {
   
    
    // parsing general ibse options
    string key = "options." + Identify();
    _energy_difference = opt->get( key + ".degeneracy" ).as< double > ();
    // number of excited states per molecule to be considered in the coupling
    _number_excitons = opt->get(key+".states").as<int> ();
    // spin types for which to determine coupling
    _spintype   = opt->get(key + ".type").as<string> ();
 
    // job tasks
    string _tasks_string = opt->get(key+".tasks").as<string> ();
    if (_tasks_string.find("input")    != std::string::npos) _do_dft_input = true;
    if (_tasks_string.find("dft")      != std::string::npos) _do_dft_run   = true;
    if (_tasks_string.find("parse")    != std::string::npos) _do_dft_parse = true;
    if (_tasks_string.find("gwbse")    != std::string::npos) _do_gwbse     = true;
    if (_tasks_string.find("coupling") != std::string::npos) _do_coupling  = true;

    // storage options
    string _store_string = opt->get(key+".store").as<string> ();
    if (_store_string.find("orbitals") != std::string::npos) _store_orbitals = true;
    if (_store_string.find("overlap") != std::string::npos) _store_overlap = true;
    if (_store_string.find("integrals") != std::string::npos) _store_integrals = true;
    if (_store_string.find("ehint") != std::string::npos) _store_ehint = true;

    // options for gwbse
    string _gwbse_xml = opt->get(key+".gwbse").as<string> ();
    //cout << endl << "... ... Parsing " << _package_xml << endl ;
    load_property_from_xml( _gwbse_options, _gwbse_xml.c_str() );
    
    
    // options for dft package
    string _package_xml = opt->get(key+".package").as<string> ();
    //cout << endl << "... ... Parsing " << _package_xml << endl ;
    load_property_from_xml( _package_options, _package_xml.c_str() );    
    key = "package";
    _package = _package_options.get(key+".name").as<string> ();
    
    // job file specification
    key = "options." + Identify() +".job";
    _jobfile = opt->get(key + ".file").as<string>();    

    
}

void IGWBSE::LoadOrbitals(string file_name, Orbitals* orbitals, Logger *log ) {

    LOG(logDEBUG, *log) << "Loading " << file_name << flush; 
    std::ifstream ifs( file_name.c_str() );
    boost::archive::binary_iarchive ia( ifs );
    try {
        ia >> *orbitals;
    } catch(std::exception &err) {
        LOG(logDEBUG, *log) << "Could not load orbitals from " << file_name << flush; 
        std::cerr << "An error occurred:\n" << err.what() << endl;
    } 
    ifs.close();

}

Job::JobResult IGWBSE::EvalJob(Topology *top, Job *job, QMThread *opThread) {
    
    // report back to the progress observer
    Job::JobResult jres = Job::JobResult();
    
    string igwbse_work_dir = "OR_FILES";
    string egwbse_work_dir = "OR_FILES";
    string frame_dir =  "frame_" + boost::lexical_cast<string>(top->getDatabaseId());     
   
    bool _run_dft_status = false;
    bool _parse_log_status = false ;
    bool _parse_orbitals_status = false;
    bool _calculate_integrals = false;
    stringstream sout;
    string output;
    
 
    // get the logger from the thread
    Logger* pLog = opThread->getLogger();   
   
    // get the information about the job executed by the thread
    int _job_ID = job->getId();
    Property _job_input = job->getInput();  
    list<Property*> segment_list = _job_input.Select( "segment" );    
    int ID_A   = segment_list.front()->getAttribute<int>( "id" );
    string type_A = segment_list.front()->getAttribute<string>( "type" );
    int ID_B   = segment_list.back()->getAttribute<int>( "id" );
    string type_B = segment_list.back()->getAttribute<string>( "type" );

    // set the folders 
    string _pair_dir = ( format("%1%%2%%3%%4%%5%") % "pair" % "_" % ID_A % "_" % ID_B ).str();
     
    path arg_path, arg_pathA, arg_pathB, arg_pathAB;
           
    string orbFileA  = (arg_pathA /  egwbse_work_dir / "molecules_gwbse" / frame_dir / (format("%1%_%2%%3%") % "molecule" % ID_A % ".orb").str()).c_str();
    string orbFileB  = (arg_pathB /  egwbse_work_dir / "molecules_gwbse" / frame_dir / (format("%1%_%2%%3%") % "molecule" % ID_B % ".orb").str()).c_str();
    string orbFileAB = (arg_pathAB / igwbse_work_dir / "pairs_gwbse" / frame_dir / (format("%1%%2%%3%%4%%5%") % "pair_" % ID_A % "_" % ID_B % ".orb" ).str()).c_str();
    string _orb_dir  = (arg_path / igwbse_work_dir / "pairs_gwbse" / frame_dir).c_str();
    
    Segment *seg_A = top->getSegment( ID_A );   
    assert( seg_A->getName() == type_A );
    
    Segment *seg_B = top->getSegment( ID_B );
    assert( seg_B->getName() == type_B );
    
    vector < Segment* > segments;
    segments.push_back( seg_A );
    segments.push_back( seg_B );
    

    LOG(logINFO,*pLog) << TimeStamp() << " Evaluating pair "  
            << _job_ID << " ["  << ID_A << ":" << ID_B << "] out of " << 
           (top->NBList()).size()  << flush; 

    string _package_append = _package + "_gwbse";
    
    string _qmpackage_work_dir = (arg_path / igwbse_work_dir / _package_append / frame_dir / _pair_dir).c_str();    
    // get the corresponding object from the QMPackageFactory
    QMPackage *_qmpackage =  QMPackages().Create( _package );
    // set a log file for the package
    _qmpackage->setLog( pLog );       
    // set the run dir 
    _qmpackage->setRunDir( _qmpackage_work_dir );
    // get the package options
    _qmpackage->Initialize( &_package_options );

    // if asked, prepare the input files
    if (_do_dft_input) {
        boost::filesystem::create_directories( _qmpackage_work_dir );
        Orbitals *_orbitalsAB = NULL;        
        if ( _qmpackage->GuessRequested() ) { // do not want to do an SCF loop for a dimer
            LOG(logINFO,*pLog) << "Guess requested, reading molecular orbitals" << flush;
            Orbitals _orbitalsA, _orbitalsB;   
            _orbitalsAB = new Orbitals();
            // load the corresponding monomer orbitals and prepare the dimer guess 
            
            // failed to load; wrap-up and finish current job
            if ( !_orbitalsA.Load( orbFileA ) ) {
               LOG(logERROR,*pLog) << "Do input: failed loading orbitals from " << orbFileA << flush; 
               cout << *pLog;
               output += "failed on " + orbFileA;
               jres.setOutput( output ); 
               jres.setStatus(Job::FAILED);
               delete _qmpackage;
               return jres;
            }
            
            if ( !_orbitalsB.Load( orbFileB ) ) {
               LOG(logERROR,*pLog) << "Do input: failed loading orbitals from " << orbFileB << flush; 
               cout << *pLog;
               output += "failed on " + orbFileB;
               jres.setOutput( output ); 
               jres.setStatus(Job::FAILED);
               delete _qmpackage;
               return jres;
            }

            PrepareGuess(&_orbitalsA, &_orbitalsB, _orbitalsAB, pLog);
        }
        _qmpackage->WriteInputFile(segments, _orbitalsAB);
        delete _orbitalsAB;
    } // end of the input
 
    
    
    // run the executable
    if ( _do_dft_run ) {
            _run_dft_status = _qmpackage->Run( );
            if ( !_run_dft_status ) {
                    output += "run failed; " ;
                    LOG(logERROR,*pLog) << _qmpackage->getPackageName() << " run failed" << flush;
                    cout << *pLog;
                    jres.setOutput( output ); 
                    jres.setStatus(Job::FAILED);
                    delete _qmpackage;
                    return jres;
            } 
    } // end of the run
    
    
    // This will be later used to write orbitals of the dimer to a file 
    // SOMETHING TO CLEANUP
    Orbitals _orbitalsAB; 
   // parse the log/orbitals files
    if ( _do_dft_parse ) {
             _parse_log_status = _qmpackage->ParseLogFile( &_orbitalsAB );

            if ( !_parse_log_status ) {
                    output += "log incomplete; ";
                    LOG(logERROR,*pLog) << "LOG parsing failed" << flush;
                    cout << *pLog;
                    jres.setOutput( output ); 
                    jres.setStatus(Job::FAILED);
                    delete _qmpackage;
                    return jres;
            } 
            
            _parse_orbitals_status = _qmpackage->ParseOrbitalsFile( &_orbitalsAB );

            if ( !_parse_orbitals_status ) {
                    output += "fort7 failed; " ;
                    LOG(logERROR,*pLog) << "Orbitals parsing failed" << flush;
                    cout << *pLog;
                    jres.setOutput( output ); 
                    jres.setStatus(Job::FAILED);
                    delete _qmpackage;
                    return jres;
            } 
    } // end of the parse orbitals/log

    
    // do excited states calculation
    if ( _do_gwbse ){
        _gwbse.setLogger(pLog);
        _gwbse.Initialize( &_gwbse_options );
        bool _evaluate = _gwbse.Evaluate( &_orbitalsAB );
        if ( !_evaluate ) { LOG(logERROR,*pLog) << "Failed GWBSE " << flush;  }
        // std::cout << *pLog;
    } // end of excited state calculation, exciton data is in _orbitalsAB

   
    // calculate the coupling
    ub::matrix<float> _JAB_singlet;
    ub::matrix<float> _JAB_triplet;
    Property _job_summary;
    Orbitals _orbitalsA, _orbitalsB;
    if ( _do_coupling ){
        // orbitals must be loaded from a file
        if ( !_do_gwbse ) LoadOrbitals( orbFileAB, &_orbitalsAB, pLog );
        

       
       // failed to load; wrap-up and finish current job
       if ( !_orbitalsA.Load( orbFileA ) ) {
               LOG(logERROR,*pLog) << "Failed loading orbitals from " << orbFileA << flush; 
               cout << *pLog;
               output += "failed on " + orbFileA;
               jres.setOutput( output ); 
               jres.setStatus(Job::FAILED);
               delete _qmpackage;
               return jres;
       }
       
        if ( !_orbitalsB.Load( orbFileB ) ) {
              LOG(logERROR,*pLog) << "Failed loading orbitals from " << orbFileB << flush; 
               cout << *pLog;
               output += "failed on " + orbFileB;
               jres.setOutput( output ); 
               jres.setStatus(Job::FAILED);
               delete _qmpackage;
               return jres;
        }
 
        
       _bsecoupling.setLogger(pLog);
       _calculate_integrals = _bsecoupling.CalculateCouplings( &_orbitalsA, &_orbitalsB, &_orbitalsAB, &_JAB_singlet, &_JAB_triplet, _spintype );   
       // std::cout << _log;
       
       if ( !_calculate_integrals ) {
                output += "integrals failed; " ;
                LOG(logERROR,*pLog) << "Calculating integrals failed" << flush;
                cout << *pLog;
                jres.setOutput( output ); 
                jres.setStatus(Job::FAILED);
                return jres;
       } 
       
    }
    
    
    
    
   LOG(logINFO,*pLog) << TimeStamp() << " Finished evaluating pair " << ID_A << ":" << ID_B << flush; 

   
   // save orbitals 
   boost::filesystem::create_directories(_orb_dir);  

   LOG(logDEBUG,*pLog) << "Saving orbitals to " << orbFileAB << flush;
   std::ofstream ofs( orbFileAB.c_str() );
   boost::archive::binary_oarchive oa( ofs );
   if ( _calculate_integrals ) {
      // adding coupling elements
      ub::matrix<float>& _J_singlets_store = _orbitalsAB.SingletCouplings();
      ub::matrix<float>& _J_triplets_store = _orbitalsAB.TripletCouplings();
      _J_singlets_store = _JAB_singlet;
      _J_triplets_store = _JAB_triplet;
      _orbitalsAB.setCoupledExcitonsA( _number_excitons );
      _orbitalsAB.setCoupledExcitonsB( _number_excitons );
   }
   // serialization of electron-hole interaction only if explicitly requested
   if ( !_store_ehint ){
       _orbitalsAB.eh_d().resize(0,0);
       _orbitalsAB.eh_x().resize(0,0);       
   }
   oa << _orbitalsAB;
   ofs.close();
   
   if ( _calculate_integrals ){
   Property *_job_output = &_job_summary.add("output","");
   Property *_pair_summary = &_job_output->add("pair","");
   Property *_type_summary = &_pair_summary->add("type","");
    if ( _spintype == "singlets" || _spintype == "all" ){
        Property *_singlet_summary = &_type_summary->add("singlets","");
        for (int stateA = 0; stateA < _number_excitons ; ++stateA ) {
           for (int stateB = 0; stateB < _number_excitons  ; ++stateB ) {
               float JAB = _bsecoupling.getCouplingElement( stateA , stateB, &_orbitalsA, &_orbitalsB, &_JAB_singlet, _energy_difference );
               Property *_coupling_summary = &_singlet_summary->add("coupling", boost::lexical_cast<string>(JAB)); 
               float energyA = _orbitalsA.BSESingletEnergies()[stateA]*27.21138386/2.0;
               float energyB = _orbitalsB.BSESingletEnergies()[stateB]*27.21138386/2.0;
               _coupling_summary->setAttribute("excitonA", stateA);
               _coupling_summary->setAttribute("excitonB", stateB);
               _coupling_summary->setAttribute("energyA", energyA);
               _coupling_summary->setAttribute("energyB", energyB);
           } 
        }
    }
    if ( _spintype == "triplets" || _spintype == "all" ){
        Property *_triplet_summary = &_type_summary->add("triplets","");
        for (int stateA = 0; stateA < _number_excitons ; ++stateA ) {
           for (int stateB = 0; stateB < _number_excitons  ; ++stateB ) {
               float JAB = _bsecoupling.getCouplingElement( stateA , stateB, &_orbitalsA, &_orbitalsB, &_JAB_triplet, _energy_difference );
               Property *_coupling_summary = &_triplet_summary->add("coupling", boost::lexical_cast<string>(JAB)); 
               float energyA = _orbitalsA.BSETripletEnergies()[stateA]*27.21138386/2.0;
               float energyB = _orbitalsB.BSETripletEnergies()[stateB]*27.21138386/2.0;
               _coupling_summary->setAttribute("excitonA", stateA);
               _coupling_summary->setAttribute("excitonB", stateB);
               _coupling_summary->setAttribute("energyA", energyA);
               _coupling_summary->setAttribute("energyB", energyB);
           } 
        }
    }    
   }
   
 
        votca::tools::PropertyIOManipulator iomXML(votca::tools::PropertyIOManipulator::XML, 1, "");
        sout <<  iomXML << _job_summary;



   // cleanup whatever is not needed
   _qmpackage->CleanUp();
   delete _qmpackage;
   
    jres.setOutput( _job_summary );    
    jres.setStatus(Job::COMPLETE);
    
    return jres;
}


void IGWBSE::PrepareGuess( Orbitals* _orbitalsA, Orbitals* _orbitalsB, Orbitals* _orbitalsAB, Logger *log ) 
{
    
    LOG(logDEBUG,*log)  << "Constructing the guess for dimer orbitals" << flush;   
   
    // constructing the direct product orbA x orbB
    int _basisA = _orbitalsA->getBasisSetSize();
    int _basisB = _orbitalsB->getBasisSetSize();
       
    int _levelsA = _orbitalsA->getNumberOfLevels();
    int _levelsB = _orbitalsB->getNumberOfLevels();
    
    int _electronsA = _orbitalsA->getNumberOfElectrons();
    int _electronsB = _orbitalsB->getNumberOfElectrons();
    
    ub::zero_matrix<double> zeroB( _levelsA, _basisB ) ;
    ub::zero_matrix<double> zeroA( _levelsB, _basisA ) ;
    
    ub::matrix<double>* _mo_coefficients = _orbitalsAB->getOrbitals();    
    //cout << "MO coefficients " << *_mo_coefficients << endl;
    
    // AxB = | A 0 |  //   A = [EA, EB]  //
    //       | 0 B |  //                 //
    _mo_coefficients->resize( _levelsA + _levelsB, _basisA + _basisB  );
    _orbitalsAB->setBasisSetSize( _basisA + _basisB );
    _orbitalsAB->setNumberOfLevels( _electronsA - _electronsB , 
                                    _levelsA + _levelsB - _electronsA - _electronsB );
    _orbitalsAB->setNumberOfElectrons( _electronsA + _electronsB );
    
    ub::project( *_mo_coefficients, ub::range (0, _levelsA ), ub::range ( _basisA, _basisA +_basisB ) ) = zeroB;
    ub::project( *_mo_coefficients, ub::range (_levelsA, _levelsA + _levelsB ), ub::range ( 0, _basisA ) ) = zeroA;    
    ub::project( *_mo_coefficients, ub::range (0, _levelsA ), ub::range ( 0, _basisA ) ) = *_orbitalsA->getOrbitals();
    ub::project( *_mo_coefficients, ub::range (_levelsA, _levelsA + _levelsB ), ub::range ( _basisA, _basisA + _basisB ) ) = *_orbitalsB->getOrbitals();   

    //cout << "MO coefficients " << *_mo_coefficients << endl;
    
    ub::vector<double>* _energies = _orbitalsAB->getEnergies();
    _energies->resize( _levelsA + _levelsB );
     
    ub::project( *_energies, ub::range (0, _levelsA ) ) = *_orbitalsA->getEnergies();
    ub::project( *_energies, ub::range (_levelsA, _levelsA + _levelsB ) ) = *_orbitalsB->getEnergies();
    
    //cout << "MO energies " << *_energies << endl;
    
    ///"... ... Have now " >> _energies->size() >> " energies\n" >> *opThread;

}   

void IGWBSE::WriteJobFile(Topology *top) {

    cout << endl << "... ... Writing job file " << flush;
    std::ofstream ofs;
    ofs.open(_jobfile.c_str(), ofstream::out);
    if (!ofs.is_open()) throw runtime_error("\nERROR: bad file handle: " + _jobfile);

 
    QMNBList::iterator pit;
    QMNBList &nblist = top->NBList();    

    int jobCount = 0;
    if (nblist.size() == 0) {
        cout << endl << "... ... No pairs in neighbor list, skip." << flush;
        return;
    }    

    ofs << "<jobs>" << endl;    
    string tag = "";
    
    for (pit = nblist.begin(); pit != nblist.end(); ++pit) {

        int id1 = (*pit)->Seg1()->getId();
        string name1 = (*pit)->Seg1()->getName();
        int id2 = (*pit)->Seg2()->getId();
        string name2 = (*pit)->Seg2()->getName();   

        int id = ++jobCount;

        Property Input;
        Property *pInput = &Input.add("input","");
        Property *pSegment =  &pInput->add("segment" , boost::lexical_cast<string>(id1) );
        pSegment->setAttribute<string>("type", name1 );
        pSegment->setAttribute<int>("id", id1 );

        pSegment =  &pInput->add("segment" , boost::lexical_cast<string>(id2) );
        pSegment->setAttribute<string>("type", name2 );
        pSegment->setAttribute<int>("id", id2 );
        
        Job job(id, tag, Input, Job::AVAILABLE );
        job.ToStream(ofs,"xml");
        
    }

    // CLOSE STREAM
    ofs << "</jobs>" << endl;    
    ofs.close();
    
    cout << endl << "... ... In total " << jobCount << " jobs" << flush;
    
}

/**
 * Reads-in electronic couplings from the job file to topology 
 * Does not detect level degeneracy! (TO DO)
 * Does not account for SUPEREXCHANGE (TO DO) 
 * 
void IDFT::ReadJobFile( Topology *top ) 
{
    Property xml;

    QMNBList &nblist = top->NBList();   
    int _number_of_pairs = nblist.size();
    int _current_pairs = 0;
    int _incomplete_jobs = 0;
    
    Logger log;
    log.setReportLevel(logINFO);
    
    // load the xml job file into the property object
    load_property_from_xml(xml, _jobfile);
    
    list<Property*> jobProps = xml.Select("jobs.job");
    list<Property*> ::iterator it;

    for (it = jobProps.begin(); it != jobProps.end(); ++it) {
 
        // check if this job has output, otherwise complain
        if ( (*it)->exists("output") && (*it)->exists("output.pair") ) {
            
            Property poutput = (*it)->get("output.pair");
            
            int homoA = poutput.getAttribute<int>("homoA");
            int homoB = poutput.getAttribute<int>("homoB");
            
            int idA = poutput.getAttribute<int>("idA");
            int idB = poutput.getAttribute<int>("idB");
                       
            string typeA = poutput.getAttribute<string>("typeA");
            string typeB = poutput.getAttribute<string>("typeB");

            //cout << idA << ":" << idB << "\n"; 
            Segment *segA = top->getSegment(idA);
            Segment *segB = top->getSegment(idB);
            QMPair *qmp = nblist.FindPair(segA,segB);
            
            // there is no pair in the neighbor list with this name
            if (qmp == NULL) { 
                LOG(logINFO, log) << "No pair " <<  idA << ":" << idB << " found in the neighbor list. Ignoring" << flush; 
            }   else {
                
                _current_pairs++;
                
                list<Property*> pOverlap = poutput.Select("overlap");
                list<Property*> ::iterator itOverlap;

                    // run over all level combinations and select HOMO-HOMO and LUMO-LUMO
                    for (itOverlap = pOverlap.begin(); itOverlap != pOverlap.end(); ++itOverlap) {

                        double energyA = (*itOverlap)->getAttribute<double>("eA");
                        double energyB = (*itOverlap)->getAttribute<double>("eB");
                        double overlapAB = (*itOverlap)->getAttribute<double>("jAB");
                        int orbA = (*itOverlap)->getAttribute<double>("orbA");
                        int orbB = (*itOverlap)->getAttribute<double>("orbB");

                        if ( orbA == homoA && orbB == homoB ) {
                                qmp->setJeff2(overlapAB*overlapAB, 1);
                                qmp->setIsPathCarrier(true, 1);
                        }

                        if ( orbA == homoA+1 && orbB == homoB+1 ) {
                                qmp->setJeff2(overlapAB*overlapAB, -1);
                                qmp->setIsPathCarrier(true, -1);
                        }
                    }    
            }
            
        } else { // output not found, job failed - report - throw an exception in the future
            _incomplete_jobs++;
            LOG(logINFO, log) << "Job " << (*it)->get( "id" ).as<string>() << " status is: " << (*it)->get( "status" ).as<string>() << endl;
        }
    }
    
    LOG(logINFO, log) << "Pairs [total:saved] " <<  _number_of_pairs << ":" << _current_pairs << " Incomplete jobs: " << _incomplete_jobs << flush; 
    cout << log;
}
*/

/** 
 * Imports electronic couplings with superexchange
 */  

void IGWBSE::ReadJobFile(Topology *top) {

    Property xml;

    vector<Property*> records;
    
    // gets the neighborlist from the topology
    QMNBList &nblist = top->NBList();
    int _number_of_pairs = nblist.size();
    int _current_pairs = 0;
    int _incomplete_jobs = 0;
    
    // output using logger
    Logger _log;
    _log.setReportLevel(logINFO);
    
    // generate lists of bridges for superexchange pairs
    nblist.GenerateSuperExchange();

    // load the QC results in a vector indexed by the pair ID
    load_property_from_xml(xml, _jobfile);
    list<Property*> jobProps = xml.Select("jobs.job");
    
    records.resize( jobProps.size() + 1  );
    
    // loop over all jobs = pair records in the job file
    for (list<Property*> ::iterator  it = jobProps.begin(); it != jobProps.end(); ++it) {
 
        // if job produced an output, then continue with analysis
        if ( (*it)->exists("output") && (*it)->exists("output.pair") ) {
            
            // get the output records
            Property poutput = (*it)->get("output.pair");
            // id's of two segments of a pair
            int idA = poutput.getAttribute<int>("idA");
            int idB = poutput.getAttribute<int>("idB");
            // segments which correspond to these ids           
            Segment *segA = top->getSegment(idA);
            Segment *segB = top->getSegment(idB);
            // pair that corresponds to the two segments
            QMPair *qmp = nblist.FindPair(segA,segB);
            
            if (qmp == NULL) { // there is no pair in the neighbor list with this name
                LOG_SAVE(logINFO, _log) << "No pair " <<  idA << ":" << idB << " found in the neighbor list. Ignoring" << flush; 
            }   else {
                //LOG(logINFO, _log) << "Store in record: " <<  idA << ":" << idB << flush; 
                records[qmp->getId()] = & ((*it)->get("output.pair"));
            }
        } else {
            throw runtime_error("\nERROR: Job file incomplete.\n Check your job file for FAIL, AVAILABLE, or ASSIGNED. Exiting\n");
        }
    } // finished loading from the file


    // loop over all pairs in the neighbor list
    std::cout << "Neighborlist size " << top->NBList().size() << std::endl;
    for (QMNBList::iterator ipair = top->NBList().begin(); ipair != top->NBList().end(); ++ipair) {
        
        QMPair *pair = *ipair;
        Segment* segmentA = pair->Seg1();
        Segment* segmentB = pair->Seg2();
        
        double Jeff2_homo = 0;
        double Jeff2_lumo = 0;
        
        cout << "Processing pair " << segmentA->getId() << ":" << segmentB->getId() << flush;
        
        QMPair::PairType _ptype = pair->getType();
        Property* pair_property = records[ pair->getId() ];
 
        int homoA = pair_property->getAttribute<int>("homoA");
        int homoB = pair_property->getAttribute<int>("homoB");
       
        // If a pair is of a direct type 
        if ( _ptype == QMPair::Hopping ||  _ptype == QMPair::SuperExchangeAndHopping ) {
            cout << ":hopping" ;
            list<Property*> pOverlap = pair_property->Select("overlap");
 
            for (list<Property*> ::iterator itOverlap = pOverlap.begin(); itOverlap != pOverlap.end(); ++itOverlap) {

                double overlapAB = (*itOverlap)->getAttribute<double>("jAB");
                int orbA = (*itOverlap)->getAttribute<double>("orbA");
                int orbB = (*itOverlap)->getAttribute<double>("orbB");
                cout << " orbA:orbB " << orbA << ":" << orbB << flush;
                if ( orbA == homoA && orbB == homoB ) {
                    Jeff2_homo += overlapAB*overlapAB;
                }

                if ( orbA == homoA+1 && orbB == homoB+1 ) {
                    Jeff2_lumo += overlapAB*overlapAB;
                }
            }    
            
        }
        
        // if pair has bridges only
        if ( _ptype == QMPair::SuperExchange  ||  _ptype == QMPair::SuperExchangeAndHopping ) {
            cout << ":superexchange" << endl;
            list<Property*> pOverlap = pair_property->Select("overlap");
            
            // this is to select HOMO_A and HOMO_B 
            // [-Wunused-but-set-variable]
            //double overlapAB;
            int orbA;
            int orbB;
            //double energyA;
            //double energyB;
            
            for (list<Property*> ::iterator itOverlap = pOverlap.begin(); itOverlap != pOverlap.end(); ++itOverlap) {
               orbA = (*itOverlap)->getAttribute<int>("orbA");
               orbB = (*itOverlap)->getAttribute<int>("orbB");
               if ( orbA == homoA && orbB == homoB ) {  
                   //[-Wunused-but-set-variable]
                   // overlapAB = (*itOverlap)->getAttribute<double>("jAB");
                   //energyA = (*itOverlap)->getAttribute<double>("eA");
                   //energyB = (*itOverlap)->getAttribute<double>("eB");
                    break;
                }
            }
            
            cout << " homoA:homoB, orbA:orbB " << homoA << ":" << homoB << "," << orbA << ":" << orbB;

            /*QMNBList::iterator nit;
            for (nit = nblist.begin(); nit != nblist.end(); ++nit) {
                
                QMPair *qmp = *nit;
                Segment *seg1 = qmp->Seg1();
                Segment *seg2 = qmp->Seg2();
                
                cout << "ID1 " << seg1->getId();
                cout << " <> ID2 " << seg2->getId() << endl;
                
            }*/
            // loop over the bridging segments
            for ( vector< Segment* >::const_iterator itBridge = pair->getBridgingSegments().begin() ; itBridge != pair->getBridgingSegments().end(); itBridge++ ) {
                        
                Segment* Bridge = *itBridge;
                int IDBridge = Bridge->getId();
                
                cout << " BridgeID:" << IDBridge;

                // pairs from the bridge to the donor and acceptor
                QMPair* Bridge_A = nblist.FindPair( segmentA, Bridge );
                if( Bridge_A == NULL ) cout << "Bridge-SegmentA pair not found" << std::endl;  

                QMPair* Bridge_B = nblist.FindPair( segmentB, Bridge );
                if( Bridge_B == NULL ) cout << "Bridge-SegmentB pair not found " << segmentB->getId() << ":" << Bridge->getId()<< std::endl;
                

                cout << " IDBA:IDBB " << Bridge_A->getId() << ":" << Bridge_B->getId();

                
                Property* pBridge_A = records[ Bridge_A->getId() ];
                Property* pBridge_B = records[ Bridge_B->getId() ];

                list<Property*> pOverlapA = pBridge_A->Select("overlap");
                list<Property*> pOverlapB = pBridge_B->Select("overlap");

                // IDs of the Donor and Acceptor
                
                //[-Wunused-variable]
                //int IdA = segmentA->getId();
                //int IdB = segmentB->getId();


                                
                // IDs stored in the file
                int id1A = pBridge_A->getAttribute<int>("idA");
                
                //[-Wunused-variable]
                //int id2A = pBridge_A->getAttribute<int>("idB");

                int id1B = pBridge_B->getAttribute<int>("idA");
                
                //[-Wunused-variable]
                //int id2B = pBridge_B->getAttribute<int>("idB");

                // suffix for the donor and acceptor 
                string suffixA = ( id1A == IDBridge ) ? "B" : "A"; // use "A" as a bridge 
                string suffixB = ( id1B == IDBridge ) ? "B" : "A"; // use "A" as a bridge 
                string suffixBridgeA = ( id1A == IDBridge ) ? "A" : "B";
                string suffixBridgeB = ( id1B == IDBridge ) ? "A" : "B";
                
                cout << " id1A:id1B " << id1A << ":" << id1B;
                
                //cout << *pBridge_A << endl;
                //cout << *pBridge_B << endl;
               
                //double check if the records are correct
                int homoBridgeA = pBridge_A->getAttribute<int>("homo" + suffixBridgeA );
                int homoBridgeB = pBridge_B->getAttribute<int>("homo" + suffixBridgeB );
                
                if ( homoBridgeA != homoBridgeB ) assert( homoBridgeA == homoBridgeB );
                int homoBridge = homoBridgeA;
               
                //exit(0);
                
                // double loop over all levels of A and B
                for (list<Property*> ::iterator itOverlapA = pOverlapA.begin(); itOverlapA != pOverlapA.end(); ++itOverlapA) {
                for (list<Property*> ::iterator itOverlapB = pOverlapB.begin(); itOverlapB != pOverlapB.end(); ++itOverlapB) {
                    
                    int orbDonor = (*itOverlapA)->getAttribute<int>( "orb" + suffixA );
                    int orbAcceptor = (*itOverlapB)->getAttribute<int>( "orb" + suffixB );
                    int orbBridgeA  = (*itOverlapA)->getAttribute<int>( "orb" + suffixBridgeA );
                    int orbBridgeB = (*itOverlapB)->getAttribute<int>( "orb" + suffixBridgeB );
                    
                    if (  orbDonor == homoA && orbAcceptor == homoB && orbBridgeA == orbBridgeB && orbBridgeA <= homoBridge) {
                        
                        double jDB = (*itOverlapA)->getAttribute<double>( "jAB" );
                        double jBA = (*itOverlapB)->getAttribute<double>( "jAB" );
                        double eA  = (*itOverlapA)->getAttribute<double>( "e" + suffixA );
                        double eB  = (*itOverlapB)->getAttribute<double>( "e" + suffixB );
                        
                        double eBridgeA  = (*itOverlapA)->getAttribute<double>( "e" + suffixBridgeA );
                        double eBridgeB  = (*itOverlapB)->getAttribute<double>( "e" + suffixBridgeB );
                        
                        //assert( eBridgeA - eBridgeB < 1e-50 );
                     
                        cout << homoA << " " << homoB << " " << (*itOverlapA)->getAttribute<int>( "orb" + suffixBridgeA )
                             << " JDB " << jDB 
                             << " JBA " << jBA << endl;
                        
                        // This in principle violates detailed balance. Any ideas?
                        Jeff2_homo += 0.5 * (jDB*jBA / (eA - eBridgeA) + jDB*jBA / (eB - eBridgeB));
                        
                                
                    }

                    if (  orbDonor == homoA+1 && orbAcceptor == homoB+1 && orbBridgeA == orbBridgeB && orbBridgeA > homoBridge) {
                        
                        double jDB = (*itOverlapA)->getAttribute<double>( "jAB" );
                        double jBA = (*itOverlapB)->getAttribute<double>( "jAB" );
                        double eA  = (*itOverlapA)->getAttribute<double>( "e" + suffixA );
                        double eB  = (*itOverlapB)->getAttribute<double>( "e" + suffixB );
                        
                        double eBridgeA  = (*itOverlapA)->getAttribute<double>( "e" + suffixBridgeA );
                        double eBridgeB  = (*itOverlapB)->getAttribute<double>( "e" + suffixBridgeB );
                        
                         // This in principle violates detailed balance. Any ideas?
                        Jeff2_lumo += 0.5 * (jDB*jBA / (eA - eBridgeA) + jDB*jBA / (eB - eBridgeB));
                        //jDB*jBA / (eB - eBridgeB);
                                
                    }
                    
                     
                }}
            } // end over bridges 
          
        } // end of if superexchange
        
        cout << endl;
                    
        pair->setJeff2(Jeff2_homo, 1);
        pair->setIsPathCarrier(true, 1);
        
        pair->setJeff2(Jeff2_lumo, -1);
        pair->setIsPathCarrier(true, -1);

    }
                    
    LOG_SAVE(logINFO, _log) << "Pairs [total:updated] " <<  _number_of_pairs << ":" << _current_pairs << " Incomplete jobs: " << _incomplete_jobs << flush; 
    cout << _log;
}

}};

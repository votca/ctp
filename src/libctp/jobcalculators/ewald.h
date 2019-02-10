#ifndef VOTCA_CTP_EWALD_H
#define VOTCA_CTP_EWALD_H


#include <votca/ctp/parallelxjobcalc.h>
#include <votca/ctp/xmapper.h>
#include <votca/ctp/xjob.h>
#include <votca/ctp/xinductor.h>
#include <votca/ctp/xinteractor.h>
#include <votca/ctp/ewald2d.h>
#include <votca/ctp/ewald3d.h>
#include <votca/ctp/pewald3d.h>
#include <votca/ctp/logger.h>
#include <boost/format.hpp>
#include <boost/timer/timer.hpp>


using boost::format;


namespace votca { namespace ctp {


template<class EwaldMethod>
class Ewald : public ParallelXJobCalc< vector<Job*>, Job*, Job::JobResult >
{
public:
    
    Ewald() {};
   ~Ewald() {};
   
    string          Identify() { return "ewald"; }
    void            Initialize(Property *);
    void            WriteJobFile(Topology *top);
    void            ReadJobFile(Topology *top);
    
    void            PreProcess(Topology *top);
    Job::JobResult  EvalJob(Topology *top, Job *job, QMThread *thread);
    void            PostProcess(Topology *top) { ; }
    
    XJob            ProcessInputString(Job *, Topology *, QMThread *);    

private:
    
    Property                      *_options;
    // MULTIPOLES DEFINITION & MAPPING
    string                         _xml_file;
    string                         _mps_table;    
    string                         _polar_bg_arch;
    XMpsMap                        _mps_mapper;
    bool                           _pdb_check;
    bool                           _ptop_check;
};


template<class EwaldMethod>
void Ewald<EwaldMethod>::Initialize(Property *opt) {

    // NOTE These options are passed on to <EwaldMethod> in ::EvalJob
    _options = opt;
    _maverick = (_nThreads == 1) ? true : false;
    
    cout << endl
         << "... ... Initialized with " << _nThreads << " threads. "
         << flush;

    string key = "options.ewald.jobcontrol";
        if ( opt->exists(key+".job_file")) {
            _jobfile = opt->get(key+".job_file").as<string>();
        }
        else {
            cout << endl;
            throw std::runtime_error("Job-file not set. Abort.");
        }    
    
    key = "options.ewald.multipoles";
        if (opt->exists(key+".mapping")) {
            _xml_file = opt->get(key+".mapping").as< string >();
        }
        else {
            cout << endl;
            throw std::runtime_error("Multipole mapping file not set. Abort.");
        }
        if ( opt->exists(key+".mps_table")) {
            _mps_table = opt->get(key+".mps_table").as<string>();
        }
        else {
            cout << endl;
            throw std::runtime_error("Background mps table not set. Abort.");
        }
        if ( opt->exists(key+".polar_bg")) {
            _polar_bg_arch = opt->get(key+".polar_bg").as<string>();
        }
        else { _polar_bg_arch = ""; }
        if (opt->exists(key+".pdb_check")) {
            _pdb_check = opt->get(key+".pdb_check").as<bool>();
        }
        else { _pdb_check = false; }
        if (opt->exists(key+".ptop_check")) {
            _ptop_check = opt->get(key+".ptop_check").as<bool>();
        }
        else { _ptop_check = false; }
    
    return;
}


template<class EwaldMethod>
void Ewald<EwaldMethod>::PreProcess(Topology *top) {
    // INITIALIZE MPS-MAPPER (=> POLAR TOP PREP)
    cout << endl << "... ... Initialize MPS-mapper: " << flush;
    _mps_mapper.GenerateMap(_xml_file, _mps_table, top);
    return;
}


template<class EwaldMethod>
void Ewald<EwaldMethod>::ReadJobFile(Topology *top) { 
    Property xml;

    const std::map<std::string, int> state = { {"e", -1}, {"n", 0}, {"h", 1} };

    Logger _log;    
    int _incomplete_jobs = 0;
    int _number_of_completed_jobs = 0;
    
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
    
    CTP_LOG(logDEBUG, _log) << "Reading energies from " << _jobfile << std::flush;
    
    // load the xml job file into the property object
    
    load_property_from_xml(xml, _jobfile);
    
    list<Property*> jobProps = xml.Select("jobs.job");

    list<Property*> ::iterator it;

    for (it = jobProps.begin(); it != jobProps.end(); ++it) {
 
        // check if this job has output, otherwise complain
        if ( (*it)->exists("output") && (*it)->exists("output.summary") ) {
            
            _number_of_completed_jobs++;
            
            // get the segment ID, type, and state
            std::string id_segtype_segstate = (*it)->get("tag").as<std::string>();
            vector<string> split_id_segtype_segstate;
            Tokenizer tok(id_segtype_segstate, ":");
            tok.ToVector(split_id_segtype_segstate);

            int segId = boost::lexical_cast<int>(split_id_segtype_segstate[0]);
            std::string segName  = split_id_segtype_segstate[1];
            std::string segState = split_id_segtype_segstate[2];
            
            Segment *seg = top->getSegment(segId);
            if ( seg->getName() != segName ) {
                CTP_LOG(logERROR, _log) << "Segment name " 
                        << segName << " in the job file is different from " 
                        << seg->getName() << " in the topology" 
                        << std::flush; 
                throw std::runtime_error("Input does not match topology.");
            } else {
                Property poutput  = (*it)->get("output.summary");
                double estat  = poutput.get("estat").as<double>();
                double eindu  = poutput.get("eindu").as<double>();
                double etotal = poutput.get("total").as<double>();
                seg->setEMpoles(state.at(segState), etotal);
            } 
            
        } else { // output not found, job failed - report - throw an exception in the future
            _incomplete_jobs++;
            CTP_LOG(logINFO, _log) << "Job " << (*it)->get( "id" ).as<string>() 
                    << " status is: " << (*it)->get( "status" ).as<string>() << endl;
        }
    }
        
 
    CTP_LOG(logDEBUG, _log) << "Done with the import of " 
            << _number_of_completed_jobs << " jobs" 
            << std::flush 
            << "Incomplete jobs: " << _incomplete_jobs << std::flush; 
   
    std::cout << _log;
    
    //assert(false && "<::ReadJobFile> NOT IMPLEMENTED");    
    return;
}


template<class EwaldMethod>
void Ewald<EwaldMethod>::WriteJobFile(Topology *top) {
    
    
    // SET UP FILE STREAM
    std::ofstream ofs;
    string jobFile = "ewald_jobs.xml";
    ofs.open(jobFile.c_str(), ofstream::out);
    if (!ofs.is_open()) throw runtime_error("Bad file handle: " + jobFile);
    
    ofs << "<jobs>" << endl;
    
    int jobCount = 0;    
    vector<Segment*>::iterator sit1;
    
    // DEFINE PAIR CHARGE STATES
    vector<string > states;
    vector<string> ::iterator vit;
    states.push_back("n");
    states.push_back("e");
    states.push_back("h");
    
    // CREATE JOBS FOR ALL SEGMENTS AND STATES
    for (sit1 = top->Segments().begin(); sit1 < top->Segments().end(); ++sit1) {
        Segment *seg1 = *sit1;

        int id1 = seg1->getId();
        string name1 = seg1->getName();
        
        for (vit = states.begin(); vit != states.end(); ++vit) {
            int id = ++jobCount;
            string s1 = *vit;
            string tag = (format("%1$d:%3$s:%2$s") % id1 % s1 % name1).str();
            
            Property input;
            Property &out = input.add("input","");
            Property *next = NULL;
            next = &out.add("segment", "");
            next->add("id", (format("%1$d") % id1).str());
            next->add("type", (format("%1$s") % name1).str());
            next->add("mps", (format("MP_FILES/%1$s_%2$s.mps") 
                % name1 % s1).str());
            
            Job job(id, tag, input, Job::AVAILABLE);
            job.ToStream(ofs,"xml");
        }
    }
    
    // CLOSE STREAM
    ofs << "</jobs>" << endl;    
    ofs.close();
}


template<class EwaldMethod>
XJob Ewald<EwaldMethod>::ProcessInputString(Job *job, Topology *top, 
    QMThread *thread) {
    
    // Input string looks like this:
    // <id1>:<name1>:<mpsfile1> <id2>:<name2>: ... ... ...
    
    string input = job->getInput().as<string>();
    vector<Segment*> qmSegs;
    vector<string>   qmSegMps;
    vector<string> split;
    Tokenizer toker(input, " \t\n");
    toker.ToVector(split);

    for (unsigned int i = 0; i < split.size(); ++i) {
                
        string id_seg_mps = split[i];
        vector<string> split_id_seg_mps;
        Tokenizer toker(id_seg_mps, ":");
        toker.ToVector(split_id_seg_mps);

        int segId = boost::lexical_cast<int>(split_id_seg_mps[0]);
        string segName = split_id_seg_mps[1];
        string mpsFile = split_id_seg_mps[2];

        Segment *seg = top->getSegment(segId);
        if (seg->getName() != segName) {
            CTP_LOG(logERROR,*(thread->getLogger()))
                << "ERROR: Seg " << segId << ":" << seg->getName() << " "
                << " maltagged as " << segName << ". Skip job ..." << flush;
            throw std::runtime_error("Input does not match topology.");
        }

        qmSegs.push_back(seg);
        qmSegMps.push_back(mpsFile);               
    }
    
    return XJob(job->getId(), job->getTag(), qmSegs, qmSegMps, top);
}


template<class EwaldMethod>
Job::JobResult Ewald<EwaldMethod>::EvalJob(Topology *top, Job *job,
    QMThread *thread) {
    
    boost::timer::cpu_timer cpu_t;
    cpu_t.start();
    boost::timer::cpu_times t_in = cpu_t.elapsed();
    
    Logger *log = thread->getLogger();    
    CTP_LOG(logINFO,*log)
        << "Job input = " << job->getInput().as<string>() << flush;
    
    // CREATE XJOB FROM JOB INPUT STRING
    XJob xjob = this->ProcessInputString(job, top, thread);    
    
    // GENERATE POLAR TOPOLOGY (GENERATE VS LOAD IF PREPOLARIZED)
    if (_polar_bg_arch == "") {
        CTP_LOG(logINFO,*log) << "Mps-Mapper: Generate FGC FGN BGN" << flush;
        _mps_mapper.Gen_FGC_FGN_BGN(top, &xjob, thread);
    }
    else {
        CTP_LOG(logINFO,*log) << "Mps-Mapper: Generate FGC, load FGN BGN from '" 
                << _polar_bg_arch << "'" << flush;
        _mps_mapper.Gen_FGC_Load_FGN_BGN(top, &xjob, _polar_bg_arch, thread);
    }
    
    // CALL THOLEWALD MAGIC
    EwaldMethod ewaldnd = EwaldMethod(top, xjob.getPolarTop(), _options, 
        thread->getLogger());
    if (_pdb_check)
        ewaldnd.WriteDensitiesPDB(xjob.getTag()+".densities.pdb");
    ewaldnd.Evaluate();
    if (_ptop_check)
        ewaldnd.WriteDensitiesPtop(xjob.getTag()+".fg.ptop", 
            xjob.getTag()+".mg.ptop", xjob.getTag()+".bg.ptop");
    
    // GENERATE OUTPUT AND FORWARD TO PROGRESS OBSERVER (RETURN)
    Property output = ewaldnd.GenerateOutputString();
    Job::JobResult jres = Job::JobResult();
    jres.setOutput(output);
    jres.setStatus(Job::COMPLETE);
    
    if (!ewaldnd.Converged()) {
        jres.setStatus(Job::FAILED);
        jres.setError(ewaldnd.GenerateErrorString());
        CTP_LOG(logERROR,*log) << ewaldnd.GenerateErrorString() << flush;
    }
    
    boost::timer::cpu_times t_out = cpu_t.elapsed();
    double t_run = (t_out.wall-t_in.wall)/1e9/60.;
    CTP_LOG(logINFO,*log)
        << "Job runtime was " << t_run << " min" << flush;
    
    return jres;
}


}}

#endif


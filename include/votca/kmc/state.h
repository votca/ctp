/*
 * Copyright 2009-2013 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __VOTCA_KMC_STATE_H_
#define __VOTCA_KMC_STATE_H_

#include <fstream>

//#include <votca/kmc/event.h>
//#include <votca/kmc/eventfactory.h>

// Text archive that defines boost::archive::text_oarchive
// and boost::archive::text_iarchive
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

// XML archive that defines boost::archive::xml_oarchive
// and boost::archive::xml_iarchive
//#include <boost/archive/xml_oarchive.hpp>
//#include <boost/archive/xml_iarchive.hpp>

// XML archive which uses wide characters (use for UTF-8 output ),
// defines boost::archive::xml_woarchive
// and boost::archive::xml_wiarchive
//#include <boost/archive/xml_woarchive.hpp>
//#include <boost/archive/xml_wiarchive.hpp>

// Binary archive that defines boost::archive::binary_oarchive
// and boost::archive::binary_iarchive
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <votca/kmc/carrier.h>
#include <votca/kmc/carrierfactory.h>

namespace votca { namespace kmc {
  
class State {
public:
    
    State(){ time = 0; }
    
    // iterator over carriers
    typedef std::vector< Carrier* >::iterator iterator;
    typedef const std::vector< Carrier* >::iterator const_iterator;
    
    iterator begin() { return carriers.begin(); }
    iterator end() { return carriers.end(); }    
    
    // Save and Load state into a file
    bool Save(std::string filename);
    bool Load(std::string filename);
    
    Carrier* AddCarrier( std::string type );  
    void AdvanceClock( double elapsed_time ) { time += elapsed_time; };

    void Trajectory_create( std::string trajectoryfile );
    void Trajectory_write( double time, std::string trajectoryfile);
    void Print_properties(int nelectrons, double fieldX, double fieldY, double fieldZ); 
   
private:
    // Allow serialization to access non-public data members
    friend class boost::serialization::access;   
    std::vector< Carrier* > carriers; 
    double time;
    //myvec field;
    
     // serialization itself (template implementation stays in the header)
    template<typename Archive> 
    void serialize(Archive& ar, const unsigned int version) {

        // version-specific serialization
        if(version == 0)  {
            ar & carriers;
            std::cout << "Serialized " <<  carriers.size() << " carriers"<< std::endl;
        } 
    }
    
};

inline bool State::Save(std::string filename){
    
    try {
        std::ofstream ofs( filename.c_str() );
        boost::archive::text_oarchive oa( ofs );
        oa << *this;
        ofs.close();
    } catch(std::exception &err) {
        std::cerr << "Could not write state from " << filename << flush; 
        std::cerr << " An error occurred:\n" << err.what() << endl;
        return false;
    } 
    return true;     
}

inline bool State::Load(std::string filename){
    
    try {
        std::ifstream ifs( filename.c_str() );
        boost::archive::text_iarchive ia( ifs );
        ia >> *this;
        ifs.close();
    } catch(std::exception &err) {
        std::cerr << "Could not load state from " << filename << flush; 
        std::cerr << "An error occurred:\n" << err.what() << endl;
        return false;
    } 
    return true;
}

// Add a carrier of a certain type
inline Carrier* State::AddCarrier( std::string type ) {

    Carrier *carrier = Carriers().Create( type );
    carrier->SetID(carriers.size()+1);
    std::cout << "Adding carrier " << carrier->Type() << " " << carrier->id();
    carriers.push_back( carrier );
    return carrier;
}


inline void State::Trajectory_create(std::string trajectoryfile){
    
    fstream trajectory;
    char trajfile[100];
    std::strcpy(trajfile, trajectoryfile.c_str());
    std::cout << "Writing trajectory to " << trajfile << "." << endl; 
    trajectory.open (trajfile, fstream::out);
 
    trajectory << "'time[s]'\t";
    
    for ( State::iterator it_carrier = carriers.begin(); it_carrier != carriers.end(); ++it_carrier ) {
        Carrier* carrier = *it_carrier;
        trajectory << "'" << carrier->Type() << carrier->id()  <<"_x'\t";    
        trajectory << "'" << carrier->Type() << carrier->id() << "_y'\t";    
        trajectory << "'" << carrier->Type() << carrier->id() << "_z'\t";      
    }
    
    trajectory << endl;
    trajectory.close();   
}

inline void State::Trajectory_write(double time, std::string trajectoryfile){
    
    fstream trajectory;
    char trajfile[100];
    strcpy(trajfile, trajectoryfile.c_str());
    trajectory.open (trajfile, fstream::out | fstream::app); 
    
    // write to trajectory file
    trajectory << time << "\t";            
    for ( State::iterator it_carrier = carriers.begin(); it_carrier != carriers.end(); ++it_carrier ) {
        Carrier* carrier = *it_carrier;
        trajectory << carrier->Position().getX() <<  "\t";
        trajectory << carrier->Position().getY() <<  "\t";
        trajectory << carrier->Position().getZ() <<  "\t";
    }
    trajectory << endl;  
    trajectory.close();
}

inline void State::Print_properties(int nelectrons, double fieldX, double fieldY, double fieldZ){
    
    //std::cout << "State has " << carriers.size() << " carriers"<< std::endl;
    
    std::cout << "Time: " << time << " seconds" << std::endl;
    Carrier* carrier;
    votca::tools::vec average_e_distance;
    
    std:: cout << std::endl << "   Carrier Distance Travelled (m): " << std::endl;
    
    for ( State::iterator it_carrier = carriers.begin(); it_carrier != carriers.end(); ++it_carrier ) {
        carrier = *it_carrier;
        
        std::cout << "       " << carrier->Type() << " " << carrier->id() 
                << " on node: " << carrier->GetNode()->id  
                << "  " << carrier->Distance()*1E-9 << std::endl;
        
        if (carrier->Type()=="electron"){
            average_e_distance += (carrier->Distance()*1E-9);
        }
    }
    if (carrier->Type()=="electron"){
        average_e_distance /= nelectrons;
        std::cout << "   Average distance travelled by the electrons: " << average_e_distance << " (m) " << std::endl;
    }
   
    votca::tools::vec velocity;
    votca::tools::vec average_e_velocity;
    
    std:: cout << std::endl << "   Carrier Velocity (m/s): " << std::endl;
    for ( State::iterator it_carrier = carriers.begin(); it_carrier != carriers.end(); ++it_carrier ) {
        Carrier* carrier = *it_carrier;
        velocity = (carrier->Distance()*1E-9/time);
        std::cout << std::scientific << "       " << carrier->Type() << " " << carrier->id() << " " << velocity << std::endl;
    }
    
    if (carrier->Type()=="electron"){
        average_e_velocity = average_e_distance/time;
        std::cout << std::scientific << "   Average velocity of the electrons: " << average_e_velocity << " (m/s) " << std::endl;
    }
    
    double mobility_x, mobility_y, mobility_z;
    double absolute_field = sqrt(fieldX*fieldX + fieldY*fieldY + fieldZ*fieldZ);
    string field_direction = "";
    double field = 0;
    
    std::cout << std::endl << "   Carrier Mobility (m^2/Vs): " << std::endl;
    for ( State::iterator it_carrier = carriers.begin(); it_carrier != carriers.end(); ++it_carrier ) {
        Carrier* carrier = *it_carrier;
        velocity = (carrier->Distance()*1E-9/time);
        
        mobility_x = (velocity.getX()/absolute_field);
        mobility_y = (velocity.getY()/absolute_field);
        mobility_z = (velocity.getZ()/absolute_field);
        
        std::cout << "       " << carrier->Type() << " " << carrier->id() << " ["  << mobility_x << "  " << mobility_y << "  " << mobility_z << "]" << std::endl;
        
        /*if (carrier->Type()=="electron"){
            average_e_mobility_x += (velocity.getX()/absolute_field);
            average_e_mobility_y += (velocity.getY()/absolute_field);
            average_e_mobility_z += (velocity.getZ()/absolute_field);
        }*/
    }
    
    double average_e_mobility = 0;
    double average_e_mobility_x, average_e_mobility_y, average_e_mobility_z;
    
    if (carrier->Type()=="electron"){

        if(fieldX != 0 && fieldY==0 && fieldZ==0) {field_direction = "X"; field = fieldX;}
        else if(fieldY != 0 && fieldX==0 && fieldZ==0) {field_direction = "Y"; field = fieldY;}
        else if(fieldZ != 0 && fieldX==0 && fieldY==0) {field_direction = "Z"; field = fieldZ;}
        
        std::cout << "  The external electric field is in the " << field_direction << " direction " << std::endl;   
    
        average_e_mobility_x = average_e_velocity.getX()/field;
        average_e_mobility_y = average_e_velocity.getY()/field;
        average_e_mobility_z = average_e_velocity.getZ()/field;
    
        if(fieldX != 0 && fieldY==0 && fieldZ==0) {average_e_mobility = average_e_mobility_x;}
        else if(fieldY != 0 && fieldX==0 && fieldZ==0) {average_e_mobility = average_e_mobility_y;}
        else if(fieldZ != 0 && fieldX==0 && fieldY==0) {average_e_mobility = average_e_mobility_z;}
         
        std::cout << "  Average electron mobility in the " << field_direction << " direction: " 
                  << average_e_mobility  << " (m^2/Vs)   ( = " << std::scientific << (average_e_mobility*1E4) << " cm^2/Vs )" << std::endl;  
        std::cout << "  Components of the average electron mobility tensor in the " << field_direction << " direction (m^2/Vs):" << std::endl;
        std::cout << "       electron_Mobility_X" << field_direction << " = " << average_e_mobility_x << std::endl;
        std::cout << "       electron_Mobility_Y" << field_direction << " = " << average_e_mobility_y << std::endl;
        std::cout << "       electron_Mobility_Z" << field_direction << " = " << average_e_mobility_z << std::endl;      
    }
    std::cout << std::endl;  

}

}} 

BOOST_CLASS_VERSION(votca::kmc::State, 0)

#endif


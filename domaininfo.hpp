#ifndef DOMAININFO_H_
#define DOMAININFO_H_

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstddef>
#include <string>
#include "math.h"
#include "NN/CellList/CellList.hpp"
#include "Grid/grid_dist_id.hpp"
#include "DMatrix/EMatrix.hpp"
#include "hash_map/hopscotch_map.h"
#include <Space/Shape/Point.hpp>
#include <utility>
#include <algorithm>
#include <string>
// using namespace std ;

 namespace TreeInputs{
 
template<typename St, int dimn> struct domainInfo{

	    	Box<dimn, St> *_domain    ;

                size_t *_sz   ;

		size_t * _bc              ;

		Ghost<dimn,St> *_g        ;

		Ghost<dimn,long int> *_gh ;

	        domainInfo(const Box<dimn, St> domain, const size_t bc[], const  Ghost<dimn, St> ghost, const size_t sz[]){	

			_domain =const_cast <Box<dimn,St>*>(&domain) ;
			_bc = const_cast<size_t*>(bc) ;	

			_sz = const_cast<size_t*>(sz) ;	
		       _g = const_cast<Ghost<dimn,St>*>(&ghost) ;
		}


       		domainInfo(const Box<dimn, St> domain, const size_t bc[], const  Ghost<dimn, long int> ghost,const size_t sz[]){
	
			_domain =const_cast <Box<dimn,St>*>(&domain) ;

                        _bc = const_cast<size_t*>(bc) ;

                        _sz = const_cast<size_t*>(sz) ;

                        _g = const_cast<Ghost<dimn,St>*>(&ghost) ;

                }

	  	domainInfo(Box<dimn, St> domain, size_t bc[], Ghost<dimn, long int> ghost, const size_t sz[]):_domain(&domain),_bc(bc),_g(ghost){

                //      _sz = sz ;
                        _sz = const_cast<size_t>(&sz) ;

                }

	       	domainInfo(Box<dimn, St> domain, size_t bc[], Ghost<dimn, St> ghost, size_t sz[]):_domain(&domain),_bc(bc), _sz(sz){

                //      _sz = sz ;
                        _g = & ghost ;

                }
		
		
		  domainInfo(Box<dimn, St> domain, size_t bc[], Ghost<dimn, long int> ghost, size_t sz[]):_domain(&domain),_bc(bc), _sz(sz){

                //      _sz = sz ;
                        _g = & ghost ;

                }


		domainInfo(Box<dimn, St> domain):_domain(&domain){

                }


	        ~domainInfo(){

		     delete _domain ;
		     delete _bc     ;
		     delete _g      ;
		
		}	

		
		inline Point<dimn,St> getSize(){
		   
		   Point<dimn,St> domain_size ;
		  
		   domain_size.get(0) = _domain->getHigh(0) - _domain->getLow(0)   ;
		   domain_size.get(1) = _domain->getHigh(1) - _domain->getLow(1)   ;
	//      
	//         T domain_size = domain.getHigh(1) - domain.getLow(1)   ;
	//	  
		   return domain_size  ;
		
		}	  

                inline St getSize_max(){

                   Point<dimn,St> domain_size = *_domain.getHigh() - *_domain.getLow()   ;


                   return max(domain_size.get(0) , domain_size.get(1))    ;

                } 
         
	 	
		inline Point<dimn, St> getLvlDist(signed int lvl){
		   
		  if(lvl > 0 ){


			return  this->getSize().get(1) / pow(2,lvl) ;
			
		  
		  }else{


			 return  this->getSize().get(1) * pow(2,lvl) ;
		 }   
			
	    }	

		
		inline Point<dimn,St> center(){

		   Point<dimn,St> Center;	

		   Center.get(0) = (_domain->getHigh(0) - _domain->getLow(0)) / 2   ;
                   Center.get(1) = (_domain->getHigh(1) - _domain->getLow(1)) / 2  ;		   

		   return Center ;
		}


		inline Point<dimn,St> GetNearCenter( openfpm::vector<Point<dimn, St>> * vect ){
			
			Point<dimn,St> Median ,dist , temp(0,0) ;
			
			for( auto point:*vect){
			 
			  dist = center() - point ;
			 
			  if(temp < dist){
			 
			   temp = dist ;
			   
			   Median = point ;   
			 
			 }
		  
			     return Median ;
		    }
		
		}
		
		
	      inline bool CheckNearCenter(Point<dimn,St> p1, Point<dimn,St> threshold){
			 
			  Point<dimn,St> Median ,dist , temp({0,0}) ;
			
			  dist = center() - p1 ;
			 
			  if( dist.get(0) <= threshold.get(0) && dist.get(1) <= p1.get(0) ){
			 
			    return true  ;  
			 
			  }
		  
			    return false ;  
		      }		      
		
		 
	  inline int GetLvlMaxNodes(int lvl){
	
                 Point<dimn, St> domain_size = this->getSize() ;

		 int indx =  this->GetSize().get(0) > this->getSize().get(1)?0:1         ; 
		  

		 int loc = floor(*_domain.getHigh(indx) / (this->getSize().get(indx) >> lvl)) ;  // /power(2,lvl)))     ;
		  	  
		 return loc   ;
		
		}
				
		
	template<typename T> inline T GetLvlSize(int lvl){
		 
		  int indx =  this->getSize().get(0) > this->getSize().get(1)?0:1 ;

		  Point<dimn, T> domain_size = this->getSize(indx)  ;
	
	 	  return domain_size >> lvl    ;
		
		}

	  template <typename T> inline void applyPeriodic(Point<dimn,T> &pos){

                       if (_bc[0] == PERIODIC &&  pos.get(0) - *_domain.getLow(0) >  *_domain.getHigh(0) - *_domain.getLow(0)){

                                pos.get(0) = pos.get(0)  - _domain.getHigh(0)  ;

                       }

                       if (_bc[1] == PERIODIC && pos.get(1) - *_domain.getLow(1) >  *_domain.getHigh(1) - *_domain.getLow(1)){

                               pos.get(1) = pos.get(1) - _domain.getHigh(1)  ;
                       }

         }


         inline bool checkBoundary(Point<dimn, St> pos){	  
		   
	//	  
		 if(_bc[0] == PERIODIC || _bc[1] == PERIODIC){  

		                   if (pos.get(0) >  *_domain.getHigh(0)  ||  
							   
					pos.get(0) < *_domain.getLow(0)   ||
							   
					pos.get(1) >  *_domain.getHigh(1)  ||

                                        pos.get(1) < *_domain.getLow(1)    		  
				    
				      ){	
					  applyPeriodic(pos) ;	 
				       }  

					      
						return  true ;


		  }else if (_bc[0] == NON_PERIODIC || _bc[1] == NON_PERIODIC){		 

			 if(pos.get(0) > *_domain.getHigh(0) || pos.get(0) < *_domain.getLow(0) ||
			    pos.get(1) > *_domain.getHigh(1) || pos.get(1) < *_domain.getLow(1)		  

			   ){
		
			 std::cerr << "__FILE__" << "__INLINE__"  << " Error !! .. point outside the domain boundary " << std::endl ;	
			
                         return false ;			 

		        }

	          }else{

		      return true ;

	          }	

        }	


	 template <typename T> inline void Info(int l){
		
		std::cout << " Domain is bounded between: [ " << _domain->getLow(0)  << "," << _domain->getHigh(0) << "] in x" << std::endl ; 
	        std::cout << " Domain is bounded between: [ " << _domain->getLow(1)  << "," << _domain->getHigh(1) << "] in x" << std::endl ;	
		std::cout << "============================================================================" << std::endl ;
                std::cout << " Domain Spacings: [ " << _sz[0] << "," << _sz[1] << "]" << std::endl ;
       	        std::cout << " Domain Size: " << this->getSize() << std::endl ;	
		std::cout << " Domain Spacing for level:" << l << "is : " << this->getLvlDist(l) << std::endl ;
	        std::cout << " Domain is:" << _bc[0] << "in x , and " << _bc[1] << "in y" << std::endl ;	
                std::cout << "Ghost layer is" << _g->getLow(0) << " in x " <<  _g->getLow(1) << " in y " << std::endl ;

	 }	 
   
     }; 
}

#endif /* domainifo_H_ */

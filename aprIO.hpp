#ifndef APRIO_H_
#define APRIO_H_

// developed by Ayman Noureldin.
// It is still under testing and developement.
// ALso for 3-D.
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstddef>
#include <string>
#include "Matrix/SparseMatrix.hpp"
#include <Grid/grid_dist_id.hpp>
#include "Vector/Vector.hpp"
#include "Grid/grid_dist_id.hpp"
#include "FiniteDifference/Laplacian.hpp"
#include "FiniteDifference/FDScheme.hpp"
#include "Solvers/petsc_solver.hpp"
#include "Vector/vector_dist.hpp"
#include "data_type/aggregate.hpp"
#include <Eigen/Dense>
#include "math.h"
#include "NN/CellList/CellList.hpp"
#include "Grid/grid_dist_id.hpp"
#include "Vector/vector_dist_subset.hpp"
#include "Operators/Vector/vector_dist_operators.hpp"
#include "Vector/vector_dist.hpp"
#include "DMatrix/EMatrix.hpp"
#include "hash_map/hopscotch_map.h"
#include <Space/Shape/Point.hpp>
#include <Vector/vector_dist.hpp>
#include "DCPSE/Support.hpp"
#include <utility>
#include "Operators/Vector/vector_dist_operators.hpp"
#include "Vector/vector_dist_subset.hpp"
#include "DCPSE/DCPSE_op/DCPSE_op.hpp"
#include "OdeIntegrators/OdeIntegrators.hpp"
#include "quadtree.hpp"
#include "TreeNode.hpp"
#include "domaininfo.hpp"

// using namespace std ;
#define EMPTY 0
#define SEED 1
#define BOUNDARY 2
#define FILLER 3
#define ASCENDANT 4
#define ASCENDANT_NEIGHBOR 5
#define PROPAGATE 6
#define IDEAL 100
	 /*typedef aggregate<double, openfpm::vector<double>> Property_type  ;
	 
	 typedef vector_dist<2, double, Property_type> dist_vector_type    ;

	 typedef vector_dist<dim,
		  T,       
		  prop,
		  CartDecomposition<dim,T>,
		 HeapMemory,
		  memory_traits_lin,
			 std::vector<Point<dim, T>,Memory,layout_base>,
			  std::vector<prop,Memory,layout_base> > vector_dist2 ;
	 
	 */

	template<typename T, typename Vector_Container, int dim> class AprIO{
	    
	   private:
	   
	   int   _lmin , _lmax     ;

	   void * reg 		   ;
	   
	   using dom = struct TreeInputs::domainInfo<T, dim> ;
	   

	   
	//   openfpm::vector<T> _pos      ;
	   
	   AprBuilder<T, dim> *_aprbuilder      ;

	   void *_dataStruct	       	        ;
		   
		
	//   template<typename Tl, typename... prop> struct par_info{
	    

	 //  aggregate<prop ...>  _prop  ;
		
	 //   	add(Point<dim,T> _pos ,  T _Ly	,T _r_Cut ,
	 //   	vect_dist_key_dx key):pos(_pos),Ly(_Ly),r_Cut(_r_Cut),key(_key){
		
	//   };
			 

	public:
	    
	    AprIO(){

	    }
	    
	//    AprTree(openfpm::vector<Point<dim,T>> points, openfpm::vector<T> L,  Box<dim, T> domain)
	    template<typename Domain, typename Ghost, typename Periodicity > AprIO(Domain domain, Ghost g, Periodicity bc)
	    {
	       AprBuilder<T,dim> builder(domain) ;
		     	
		reg = new dom(domain,g,bc) ;	          

            }	     

	//  }
	    

	    template<typename particle>AprIO(particle par):_dataStruct(&par)
	    {
	   //  par.getDecomposition().get

	    }


            template<typename Grid> AprIO(Grid grid):_dataStruct(&grid)
            {
           //  par.getDecomposition().get

            }

	    
	    ~AprIO(){

	   	delete _aprbuilder ;
		delete _dataStruct     ;
	    }
		 
	   

	   /* 
	    template<typename Tl,typename key_type, typename particle> std::vector<pair<key_type, pair<Point<dim,T1>,T1>>>  BuildParVectors(particle par){
	     
	     std::vector< pair<key_type, pair<Point<dim,T>,T>>> par_vec ; 
		
	     auto it = par.getDomainIterator() ;
	     
	     while(it.isNext()){
		
		auto key = it.get() ;
		
		par_vec.push_back(make_pair(key, make_pair((par.getPos(key)- region.domain.getLow()) - (region.getSize() / 2 - region.domain.getLow()) , par.template get<0>(key))) ;
		
		++it ;

		}

		
		return 	par_vec  ;
		
		}

	       */
	     
		/*  
		
		 bool sortbysecdesc(const pair<vect_dist_key_dx,T> &a,
			   const pair<vect_dist_key_dx,T> &b)
			{		
			     return a.second> b.second   ;
			} 
			
		 */
	    
	    /*
	      template<typename T1, typename key_type, int dim > void SortParVec(std::vector< pair<key_type, pair<Point<dim,T1>,T1>>> &par_vec){
		 
	       // sorting particles based on their proximity to the center of the domain.       	 
		 sort(par_vec.get(0).begin(), par_vec.get(0).end(), []( Pair<key_type key, Pair<Point<dim, T>,T>> p1 , Pair<key_type key, Pair<Point<dim, T>,T>> p2 ){ p1.second.second > p2.second.second ; })  ; // sortbysec     
	      // 	 sort(par_Info.get(1).begin(), par_Info.get(1).end(), [](Point<dim, T> p1, Point<dim, T> p2){ p1 < p2 ; } )  ; // sortbysec     
	      // 	 sort(par_Info.get(2).begin(), par_Info.get(2).end(), [](Point<dim, T> p1, Point<dim, T> p2){ p1 < p2 ; } )  ; // sortbysec  
		  
	       }
	      
	       template<typename T1> int CalculateMaxLvl(openfpm::vector<T1> LMF){  
	      
		 int lmax = max(domain_size[0], dom.domain_size[1])/min(LMF) ;
	      
		  return lmax ;
	      }
	      */
/*		
	       template<typename Tl> int CalculateLvl(Tl L){  
	      
		 int lvl = ceil(log(_qtree->getDom()->getSize_max() / L))  ;
		 
		 return lvl ;
	      
	       }

*/

/// still under development ...
//
	       template<typename Qtree, typename Grid> Qtree GridTreeMapping(const Grid grid){
		       
		       size_t sz = grid.size() ;
		       auto it_grid = grid.getGridIterator(sz) ;
		 	grid_sm<dim,void> info(sz);
			while(it_grid.isNext()){
					
			}	


	       } 


// Still under development to add Mway-trees 
 /* 
 
template<typename T, typename particle> class MwayTree{
	
	private:
	
	
	public:
	
	
};



template<typename St>struct M_Way_node{

 struct M_Way_node* child[MAX + 1] ;

 int value[MAX + 1] 		   ;
  
 int count 			   ;
    
 openfpm::vector<int> keys  	   ;

} ;

*/

#endif

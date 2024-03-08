#ifndef APRBUILDER_H_
#define APRBUILDER_H_

// Developed by Ayman Noureldin
// This is a APR-tree. It should be able to perform insertion, searching, and Pulling scheme.
// This file is still under testing and developement.
// Also for 3-D, and particle distribution.
//
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstddef>
#include <string>
#include "math.h"
#include "hash_map/hopscotch_map.h"
#include <utility>
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

	template<typename T, int dim> class AprBuilder{
	    
	   private:
	   
	   int   _lmin , _lmax     ;

	   using dom = struct TreeInputs::domainInfo<T, dim> ;


 	   
	   AprTree<T, dim> *_aprtree	       ;
	  

	public:
	    
	    AprTree(){
				
	    }
	    
	    AprBuilder(  Box<dim, T> domain)
	    {
		     QuadTree<T,particle,dim> AprBuilder(domain) ;

		     _qtree = &qtree	    ;


            }	     

	    

	    AprBuilder(particle par):_par(&par)
	    {
	    

	    }
	    
	    ~AprBuilder(){

	   	delete _aprtree ;
		delete _par ;
	    }
		 
	     
		
	       template<typename Tl> int CalculateLvl(Tl L){  
	      
		 int lvl = ceil(log(_qtree->getDom()->getSize_max() / L))  ;
		 
		 return lvl ;
	      
	       }


	       template<typename Qtree, typename Node> Node* LvlMapping(Node * root){
		

		      if(root->parent){
				
			      root->parent->_ID == FILLER ;
			      return aprtree.getTree()->SearchTree(root->parent) ;
		      }else{
		              return LvlMapping(root->parent) ;		      
		      }	      

	       } 


               template<typename QtreeIter, typename Node>  auto  collectSeedNeighborhood(Node* seed, int Nebrhood)->decltype(neigh){

		 std::vector <decltype(aprtree.GetTree()->begin())> neigh ;

  			      
		   neigh =  getNearestNighbors(seed, aprtree.GetTree(),i) ;
			
		for(int i = 0; l < neigh.size(); i++){

			if (aprtree.getTree()[neigh[i]] == EMPTY){
					while(l >= 1){
						
					    auto It_parent  = & LvlMapping<Qtree,Node>(root) // aprtee.getTree()->ParentSearch(neight[i],FILLER) ;

					    if (It_parent){
					       
					       neigh.erase(i) ;					       
				               neigh[i] = It_parent ;
					       aprtree.getTree()->TreeOffset<std::vector<QtreeIter>,Node>(neigh,neigh.begin()) ;
					       neigh[0] = seed ;	       
					       neigh.shrink_to_fit() ;   
					 
					       break ;
                                             }

					    l = l - 1 ;

					}
		         	}	

	         	}	

			return neigh ;
		  }

		
	  template<typename Vector> int calculateNeighbDist(Vector &seeds ){
		
                   node * dist ;
		   int N = 0  ;

		  for(auto i = 0 ; i < seeds - 1 ; i++ ){
			
		   dist = dist +  seeds[i + 1] - seeds[i] ;   	
                   N = N + 1 ;
		  }
			
		   return  ceil(dist->pos / N) ;	  
	   }	  
	
	      
	  template<typename Qtree, typename Node> void checkNeghbrhodOverlap(){
           	
	       	  
	  }	   


	   template<typename Qtree, typename Node> int collectpoints(){
                 
	         std::array<Node*, dim> seed_nodes ;
      		 
                 std::vector <decltype(aprtree->GetTree()->begin())> Neighborhood ;
  
              for(int l = lmax; l == 1; l--){

                      for(auto i = 1; i < 1 >> l ;i++){
			
		 	 seed_nodes.push_back(SearchOnLvl<decltype(aprtree->GetTree()->begin())>(l,SEED, i) ;
			
		      }

			int Nebrhood = calculateNeighbDist<std::vector<decltype(aprtree->GetTree()->begin())>(seed_nodes) ; 
   
     			for(auto i = 0; i < seed_nodes.size() ;i++){

				Neighborhood = collectSeedNeighborhood(seed_nodes[i], Nebrhood) ;
				aprtree.getTree()[Neighborhood[]] =
			}

                   neigh =  getNearestNighbors(seed, aprtree->GetTree(),i) ;

                  for(int i = 0; l < neigh.size(); i++){

                        if (aprtree.getTree()[neigh[i]] == EMPTY){
                                        while(l >= 1){

                                            auto It_parent  = & LvlMapping<Qtree,Node>(root) // aprtee.getTree()->ParentSearch(neight[i],FILLER) ;

                                            if (It_parent){

                                               neigh.erase(i) ;
                                               neigh[i] = It_parent ;
                                               neigh.shrink_to_fit() ;

                                               break ;
                                             }

                                            l = l - 1 ;

                                        }
                                }

                        }
                }	

       }


	      template<typename Node, typename Vector> void Execute(int lmax){

		
		_aprtree->PullingScheme<>(lmax, _aprtree->GetTree()) ;
                
		auto particle_cells = collectpoints() ;
				
	      }	       


	       template<typename Node, typename Vector> Vector collectpoints(){
		
		  Vector Three_nodes_set ;

                    for(int l = lmax; l == 1; l--){

                      for(auto i = 1; i < 1 >> l ;i++){
			Node * root_seed =  SearchOnLvl<decltype(_aprtree.GetTree()->begin())>(l,SEED, i) ;
			Node * root_boundary = SearchOnLvl<decltype(_aprtree.GetTree()->begin())>(l,BOUNDARY, i) ;
			 if(root_seed || root_boundary	|| SearchOnLvl<decltype(aprtree.GetTree()->begin())>(l,FILLER,i)){
                                 
			     if(root_seed){
			
 			 	 Node * child  = _aprtree->GetTree()->ChildSearch(root_seed) ;				 
                                   _aprtree.GetTree()->Insert(child->_pos,child->_lvl + 1,SEED) ;
				   _aprtree.GetTree()->RemoveLeaf<typename T1, typename decltype(_aprtree.GetTree())>(root_seed, 
						  _aprtree.GetTree()) ;
                              

			 }else if(root_boundary) {


                                         _aprtree.GetTree()->Insert(root->_pos,s_node->_lvl + 1, BOUNDARY) ;
					 _aprtree.GetTree()->RemoveLeaf<typename T1, typename decltype(_aprtree.GetTree())>(root_boundary,
                                                  _aprtree.GetTree()) ;

                                 }
 
			 }	   
				   
				  
                              	 Three_nodes_set.push_back(SearchByLvl<decltype(aprtree.GetTree()->begin())>(l,IDEAL, i )) ;
			
                      } 
	          }	

	      }
		
    		return Three_nodes_set ;		    

	   }	  


	    
	  // In case you prefer using nodes allocated as pointers in the tree.
	  // PLEASE CHECK this function carefully  !!
	  //   


       template<typename T1> void Clear(){
   	 
   	 _aprtree->Clear()   ;

       }
   	

  
 
};
 
#endif

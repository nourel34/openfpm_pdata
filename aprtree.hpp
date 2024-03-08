#ifndef APRTREE_H_
#define APRTREE_H_

// Developed by Ayman Noureldin.
// Still under testing and development.
// Also for 3-D.
//
//
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstddef>
#include <string>
#include "data_type/aggregate.hpp"
#include "math.h"
#include "hash_map/hopscotch_map.h"
#include <Space/Shape/Point.hpp>
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



	template<typename T, int dim> class AprTree{
	    
	   private:
	   
	   int   _lmin , _lmax    	 ;

	   using dom = struct TreeInputs::domainInfo<T, dim> ;

           using node = struct TreeInputs::Node<T, dim>      ; 
	   
	   QuadTree<T,dim> *_qtree   ;
	   
//	   particle *_par	     ; 
		
	   dom reg ;		 

	public:
	    
	    AprTree(){
				
	    }
	    
	//    AprTree(openfpm::vector<Point<dim,T>> points, openfpm::vector<T> L,  Box<dim, T> domain)

	    AprTree( Box<dim, T> domain )
	    {
		   _qtree = new QuadTree<T,dim>(domain)   ;
		   reg(domain) ;

            }	     

	    
//	    AprTree( particle par ):_par(&par)
//	    {
	    
//	    }
	    
	   ~AprTree(){

	   	delete _qtree ;
//		delete _par ;
	    }
		 
	   // aggregate<std::vector<pair<vector_dist_key_dx, Ly>>, std::vector<pair<Point<dim,T>, Ly>>, std::vector<pair< r_cut, Ly>>> 
	   


	       template<typename Tl> int CalculateLvl(Tl L){  
	      
		 int lvl = ceil(log(_qtree->getDom()->getSize_max() / L))  ;
		 
		 return lvl ;
	       }
		
               
	        template<typename Tl> int GetTree(){

                 return _qtree->getTree() ;

               }


	      
	      template<typename Tl> void InsertTreeSeeds(Point<dim,Tl> pos, Tl L){
		
		 int l = CalculateLvl(L) ;
				 
		 _qtree->Insert(pos, l,SEED)    ;          				
			
	       }


      	      template<typename Qtree, typename Node> void CreateOtherleafs(Qtree qtree){
		
		  for(auto It = qtree.getTree()->begin(); It < qtree.getTree()->end(); It++){

			Node * s_node  = qtree.getTree()[It];
         
	 		Point<dim, T> leaf_pos ;  
			
			   if(s_node.children_Num){

                                auto leafs = s_node->getMissingLeafs<typeid(s_node.getLeafs().name())>(s_node.getLeafs()) ;

                                 for(auto root : leafs){

                                         qtree.getTree()->Insert(root->_pos,s_node->_lvl + 1, EMPTY) ;

                                 }
                            }
		      }
	        }		
	      


	    template<typename Qtree> void SetBoundaryNodes(int l, Qtree qtree){

                   int i = 0 ;

                   std::vector<decltype(qtree.getTree()->begin())> neigh ;


                    for(int i = 1  ; i < qtree.lvl_counter(l + 1) ; i++){
                     node s_node =  qtree.SearchOnLvl(l,IDEAL, i) ;
		     if(s_node < *(qtree.getTree()->end())){
			     // // check that the end() gives a null node
                     if(s_node->_ID == SEED || s_node->_ID == PROPAGATE){

                             neigh =  qtree.getNearestNighbors(*s_node, qtree, 1) ;

                             for(int k = 0 ; k < neigh.size(); k++ ){
                                 if( qtree.getTree()[neigh[k]]->_ID == EMPTY){
                                        qtree.getTree()[neigh[k]] = BOUNDARY ;
                                 }
                         }

                        if(s_node->parent->_ID == EMPTY){ 
			   	s_node->parent->_ID = ASCENDANT ;
				&qtree.ParentSearch(s_node) = &s_node->parent ;
			

                        }else if(s_node == ASCENDANT ){

                        if(s_node->parent->_ID == EMPTY){ 
				s_node->parent->_ID = ASCENDANT ;
				&qtree.ParentSearch(*s_node) = &(*s_node->parent) ;

                       }
                   }
               }
            }else break ;
         }
      }	    	     
	
	      template<typename Qtree> void InsertBoundaryNodes(int l, Qtree qtree){   
		      
		   
		   int i = 0 ;

		   std::vector<Point<dim,T>> neigh ;

		     
                    for(int i = 1  ; i < qtree.lvl_counter(l + 1) ; i++){
		     node* s_node =  qtree.SearchOnLvl(l,IDEAL, i) ;
		     if(s_node < *(qtree.getTree()->end())){
		     	if(s_node->_ID == SEED || s_node->_ID == PROPAGATE){
                        
			      neigh =  qtree.getNearestNeighborPos(s_node, qtree, 1) ;
			
			      for(int k = 0 ; k < neigh.size(); k++ ){
			      node * neigh_inserted  =   qtree.Insert(neigh[k], l, BOUNDARY) 	      ;
			      node *parent =   ParentSearch(neigh_inserted) ; // chec
			      parent->GetLeafLoc(parent->_pos,neigh[k]) = Search(neigh_inserted) ; // Search(neigh[k],BOUNDARY,l) ;
			      parent->children_Num =+1 ;
			      if (neigh_inserted == NULL) std::cerr << " Insertion error "  << std::endl ;
			 }
		      
			      node * asc_inserted = qtree.Insert(s_node->parent->_pos, l, ASCENDANT) ;
			      asc_inserted->GetLeafLoc(asc_inserted->_pos,s_node->_pos) = qtree.Search(asc_inserted->_pos, l,ASCENDANT) ;
                              asc_inserted->children_Num =+1 ;

                        }else if(s_node == ASCENDANT ){
			
			      node * asc_inserted  = qtree.Insert(s_node->parent->_pos, l,ASCENDANT)   ;
			       if (asc_inserted == NULL) std::cerr << " Insertion error "  << std::endl ;
			       asc_inserted->GetLeafLoc(asc_inserted->_pos,s_node) = qtree.Search(asc_inserted->_pos, l,ASCENDANT) ;
			       s_node->parent->children_Num =+1 ;
			       qtree.LvlCounter[s_node->_lvl] =- 1 ;
			       qtree.RemoveLeaf(s_node,qtree.getTree()) ;
	                }
		      }	else break ;        
		   }	
		}  		     
		      

	      
	     template<typename Qtree> void SetFillerNodes(int l, Qtree qtree){ 
	     
		   int i = 0 ;

                    for(int i = 1  ; i < qtree.lvl_counter(l + 1) ; i++){
                     node* s_node =  qtree.SearchOnLvl(l,IDEAL,i) ;                
                     if(s_node < *(qtree.getTree()->end())){
		     if(s_node->_ID == ASCENDANT_NEIGHBOR || s_node->_ID == PROPAGATE ){
                                auto It = &(qtree.ChildSearch(s_node)) ;

                       		 if(*It->_ID == EMPTY){ 
					 *It->_ID = FILLER ;
					 s_node->GetLeafLoc(s_node->_pos,*It->_pos) = *It ;
					 qtree.getTree()[It] = *It ;


                    		 }
                 	    }
               	       }else break ;
                   }
	      }

	     
	     template<typename Qtree, typename Node> void InsertFillerNodes(int l, Qtree qtree){

                   int i = 0 ;

		//
		// Fix the number per level
	             for(int i = 1  ; i < qtree.lvl_counter(l + 1) ; i++){		     
                     Node* s_node =  qtree.SearchOnLvl(l,IDEAL,i) ;
		     if(s_node < *(qtree.getTree()->end())){
                     if(s_node->_ID == ASCENDANT_NEIGHBOR || s_node->_ID == PROPAGATE ){
			   if(s_node->children_Num){
                     	 	     aggregate<Point<dim,T>, int> dist ;
    			 	     dist.get(0) = reg.getDist(s_node->_lvl + 1) ;
			             dist.get(1) = 1;
	  			     node* child = s_node->getChild(dist)  ;
				     qtree.getTree()->Insert(child->_pos, child->_lvl, FILLER) ;
				     qtree.RemoveLeaf(s_node,qtree.getTree()) ;
				     qtree.LvlCounter[s_node->_lvl] =- 1 ;
                            }
                       }

                   }else break ;
              }
	  }

		   
            template<typename Qtree> void SetAscendantNeighNodes(int l, Qtree qtree){

                 int i = 0 ;
		 std::vector<decltype(qtree.getTree()->begin())> neigh ;
                    for(int i = 1  ; i < qtree.lvl_counter(l + 1) ; i++){
                     node* s_node =  qtree.SearchOnLvl(l,IDEAL,i) ;
                     if(s_node < *(qtree.getTree()->end())){
		 	 if(s_node->_ID == ASCENDANT ){

                            neigh =  qtree.getNearestNeighborIter(s_node, qtree, 1) ;

   			    for(int k = 0 ; k < neigh.size(); k++ ){
                                 if(*neigh[k]->_ID == EMPTY){
					 
                                        *neigh[k]->_ID = ASCENDANT_NEIGHBOR ;
					node* asc_neigh_node = *neigh[k] ;
					qtree.getTree()[neigh[k]] = &(asc_neigh_node) ;
				       // check the results of this   
				

				 }else if( *neigh[k]->_ID == SEED){
					*neigh[k]->_ID = PROPAGATE ;
					 node* p_node = *neigh[k] ;
                                         qtree.getTree()[neigh[k]] = &(p_node) ;
				       // check the results of this
                                 }
                            }
                       }

		       qtree.LvlCounter[s_node->_lvl] =- 1 ;
		       qtree.RemoveLeaf(s_node,qtree.getTree()) ;

                   }else break ;
               }
           }
            
	
            template<typename Qtree> void InsertAscendantNeighNodes(int l, Qtree qtree){

                 int i = 0 ;
                 std::vector<Point<dim,T>> neigh ;
            //     while(!qtree.getTree()->end()){
                    for(int i = 1  ; i < qtree.lvl_counter(l + 1) ; i++){
          //          for(int i = 0 ; i < 1 >> l; i++){
                     node* s_node =  qtree.SearchOnLvl(l,IDEAL, i) ;
	     	     if(s_node < *(qtree.getTree()->end())){
                     if(s_node->_ID == ASCENDANT ){

                            neigh =  qtree.getNearestNeighborPos(s_node, qtree, 1) ;

                            for(int k = 0; k < neigh.size(); k++){
			       auto It =  qtree.getTree()->Search(neigh[k]->_pos,SEED,l) ;
			         if(It){
                                        qtree.getTree()[It]->_ID  = PROPAGATE ;
					qtree.getTree()[It]->parent->GetLeafLoc(
							qtree.getTree()[It]->parent->_pos,qtree.getTree()[It]->_pos
							) = qtree.getTree()[It] ;
				       qtree.getTree()[It]->parent->children_Num =+ 1 ;	
                                 }else{ 
			//		 if(qtree.getTree()->Search(neigh[k]->_pos,ASCENDANT_NEIGHBOR,l)){          
					node* asc_neigh_inserted  = qtree.getTree()->Insert(neigh[k]->_pos, s_node->_lvl,ASCENDANT_NEIGHBOR) ;
					auto It_parent = &(ParentSearch(asc_neigh_inserted)) ;
					qtree.getTree()[It_parent]->GetLeafLoc(*It_parent->_pos,asc_neigh_inserted->_pos) = asc_neigh_inserted ;
                              		qtree.getTree()[It_parent]->children_Num =+1 ;
        			//	qtree.getTree()->(&Search(*It_parent)) = It_parent ; 
					if (asc_neigh_inserted == NULL ) std::cerr << " Insertion error "  << std::endl ;

					// auto It = qtree.getTree()->Search(neigh[k],ASCENDANT_NEIGHBOR,l) ;
             
                            }
                       }
                   }
                                        qtree.LvlCounter[s_node->_lvl] =- 1 ;
			                qtree.RemoveLeaf(s_node,qtree.getTree()) ; 
               }else break ;
           }	
        }
     
         
	    template<typename Qtree> void PullingScheme(int lmax, Qtree qtree){

                     int i = 0 ;

                     for(int l = lmax - 1 ; l == 1 ; l++){ 
			
				SetAscendantNeighNodes(l, _qtree) ;

				SetFillerNodes(l, _qtree) ;
		 	
		      while(l != lmax){
			
		      		SetBoundaryNodes(l, _qtree) ;

		   	 }
              	     }
                 }
                
    

 
};
 
 
#endif

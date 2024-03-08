#ifndef TREENODE_H_
#define TREENODE_H_

// developed by Ayman Noureldin.
// Still under testing and development.
// Also for 3-D.
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstddef>
#include <string>
#include "data_type/aggregate.hpp"
#include <Eigen/Dense>
#include "math.h"
#include <Space/Shape/Point.hpp>
#include <utility>
#include "domaininfo.hpp"
#include <type_traits>
#include <typeinfo>
#include <utility>

//using namespace std ;
#define EMPTY 0
/*
#define SEED 0
#define BOUNDARY 1
#define FILLER 2
#define ASCENDAT 3
#define PROPAGATE 4
*/

namespace TreeInputs{

template<typename T, int dim> struct Node{
   
   
   Point<dim,T> _pos    ;
   
   int _lvl, _ID, key	;
   
   T _rCut              ;

   int children_Num = 0 ;
    
   
   // 2-D
   struct Node<T, dim> * parent ;

   struct Node<T,dim> * right, * left, * topright, * bottomright, * topleft, * bottomleft ;

  // 3-D
    struct Node<T, dim> * frontright,* frontleft, * backright, *backleft ;
    struct Node<T, dim> * fronttopright, * fronttopleft, * frontbottomright , *frontbottomleft ;
     struct Node<T,dim> * backtopright, * backtopleft, * backbottomright , * backbottomleft ;
 
   Node(){

   }

   Node(int Num):children_Num(Num){
        
	if (_lvl == 0){
	   
	   right = NULL ;
           left  = NULL ;
	   
	}else{
	 
	   topright = NULL ;
	   bottomright = NULL ;  
	   topleft = NULL ;
	   bottomleft = NULL ;
         
        }
   }
   

   Node(Point<dim,T> pos, int id, int l):_pos(pos),_ID(id),_lvl(l){

	if (_lvl == 0){

           right = NULL ;
           left  = NULL ;

        }else{

           topright = NULL ;
	   bottomright = NULL ;
           topleft = NULL ;
           bottomleft = NULL ;

          }
      }
   
   
   ~Node(){
	if (dim == 2){
      if (_lvl == 0){
      
      delete parent;	      
      delete right;
      delete left;

      }else{
      delete parent ;
      delete topright ;
      delete bottomright ;	      
      delete topleft ;
      delete bottomleft ;
      }
   }
 }

     template <typename St> void operator=(struct Node<St, dim>* root){
	
	  //   std::is_rvalue_reference<T>{}
                 this->_pos = root->pos ;
                 this->_lvl = root->_lvl ;
                 this->_ID = root->_ID ;
                 this->children_Num = root->children_Num ;

                 if(this->_lvl == 0){

                    this->right = root->right ;
                    this->left = root->left ;

                 }else{

                    this->topright = root->topright ;
                    this->bottomright = root->bottomright ;
                    this->topleft = root->topleft ;
                    this->bottomleft = root->bottomleft ;
                }
         }


     template <typename St> bool operator==(struct Node<St, dim>* root){

                 if (this->_pos = root->pos && this->_lvl = root->_lvl && this->_ID == root->_ID ){
                        return true ;
                 }else{
                        return false ;
                 }
         }

     template <typename St> struct Node<St,dim>* operator+(struct Node<St,dim>* root){

                struct Node<St, dim> * node ;

                node->_pos = this->_pos + root->pos ;

                node->_lvl = root->_lvl ;

                return node ;
            }

         template <typename St> struct Node<St,dim>* operator-(struct Node<St,dim>* root){

                struct Node<St, dim> * node ;

                node->_pos = this->_pos - root->pos ;

                node->_lvl = root->_lvl ;

                return node ;
          
          }


   struct Node<T,dim> * rvalGetLeafLoc(Point<dim, T> p1, Point<dim, T> p2){
	
	Point<dim, T>  loc = p2 - p1	; 

               if(loc.get(0) > 0 ){ 

        	return this->right ;
        
	}else 	return this->left  ;
        

	// CHECK IF DIR SHOULD BE A DOUBLE INSTEAD OF INTEGER
             	 
        //  double dir = static_cast<int>() ;
	double dir = atan(loc.get(1) / loc.get(0))    ; // use the angle of the distance between the input point and the center which quarter the point within
         
             
          if (0<= dir < 90){
         //   child_type = "topright" ;  
            return this->topright ; // *child_type 	   ; // ToprightTree 
                        
  	 }else if (90 <= dir < 180){
         //   child_type = "topleft" ; 
            return this->topleft ; // *child_type	   ; //TopLeftTree ; 
          
	 }else if (180 <= dir < 270){ 
           // child_type = "bottomright" ;
            return this->bottomright ; // *child_type	   ; //BottomrightTree ; 
          
	 }else if (270 <= dir <= 360 ){
           // child_type = "bottomleft" ;
            return this->bottomleft ; // *child_type      ; // BottomLeftTree ; 
          
          }
          return NULL ;
      }

        Node*& f(Node*&& a1)
        {
                return new Node(std::forward<decltype(a1)>(a1)) ;
        }

   

   struct Node<T,dim> *& lvalGetLeafLoc(Point<dim, T> p1, Point<dim, T> p2){
	
	Point<dim, T>  loc = p2 - p1	; 
        
        if(p2.get(0) > p1.get(0) && p2.get(1) == 0){
        
        	return this->right ;
        
	}else if(p2.get(0) < p1.get(0) && p2.get(1) == 0){
       
       		return this->left  ;
        }

	// CHECK IF DIR SHOULD BE A DOUBLE INSTEAD OF INTEGER
             	 
         int dir = static_cast<int>(atan(loc.get(0) / loc.get(1)))    ; // use the angle of the distance between the input point and the center which quarter the point within
         
	  char* child_type ; 
          switch(dir){
             
          case 45:
         //   child_type = "topright" ;  
            return this->topright ; // *child_type 	   ; // ToprightTree 
                        
          case 135:
         //   child_type = "topleft" ; 
            return this->topleft ; // *child_type	   ; //TopLeftTree ; 
          
          case 225:
           // child_type = "bottomright" ;
            return this->bottomright ; // *child_type	   ; //BottomrightTree ; 
          
          case 315:
           // child_type = "bottomleft" ;
            return this->bottomleft ; // *child_type      ; // BottomLeftTree ; 
          
          }
        //  return NULL ;
      }

 
 	inline struct Node* getNode(){
                return std::forward<struct Node*>(this) ;
         }


         inline void setNode( struct Node* root ){
		this == root ;
	 }	 

	
	 inline bool checkNode(struct Node* root ){
	 	
		 if(this == root){
		 return true ;
		 
		 }else return false;
	 }



        inline std::vector<struct Node<T, dim>*> GetOtherLeafLoc(struct Node<T, dim>* root){
                   
	   	std::vector<struct Node<T, dim>*> vn = {this->topright, this->bottomright,this->topleft, this->bottomleft} ;

			 if(this->_lvl == 0){

	         		vn = {this->right, this->left} ;
				
	 		 }	 

                 
		  auto It = vn.begin() ;

                  std::cout << " 1 " << std::endl ;

		  while(It < vn.end()){

			std::cout << " 2 " << std::endl ;
       		  	It = find(It, vn.end(),[root](Node * p){ if(p->_pos == root->_pos && p->_lvl == root->_lvl && p->_ID == root->_ID) return true ;}) ;
        	  	vn.erase(It) ;
			It++ ;
		  }

		  std::cout << "finished successfully " << std::endl ;
         
		  return vn ;
         }


      inline int IsOtherLeafexist(struct Node<T, dim> * root){

	 int count = 0 ;
	 
    	 std::vector<struct Node<T, dim>*> vn = {this->topright, this->bottomright,this->topleft, this->bottomleft} ;

                         if(this->_lvl == 0){

                                vn = {this->right, this->left} ;

                         }

          auto It = find(vn.begin(), vn.end(), root) ;
        	
		 for( auto it = vn.begin(); it < It ; it++ ){
               		 
            		 count++ ;
		 }  

 	   std::cout << "finished successfully " << std::endl ;			 
         
  	        return count  ;
      }




/*	Node*& f(Node*&& a1)
	{
   		return new Node(std::forward<decltype(a1)>(a1)) ;
	}
*/

        inline struct Node*& lvalGetLeaf(int i){


          if( i <= this->children_Num){

            if(this->right && i == 0) return this->right   ;
            else if(this->left && i == 1) return this->left   ;
            else if(this->topright) return this->topright ;
            else if(this->bottomright && i == IsOtherLeafexist(this->bottomright)) return this->bottomright ;
            else if(this->topleft && i == IsOtherLeafexist(this->topleft)) return this->topleft ;
            else if(this->bottomleft && i == IsOtherLeafexist(this->bottomleft)) return this->bottomleft ;
          //  else  return NULL ;

          }else{

            std::cerr << "__FILE__" << ":" << "__LINE__" <<  " This Leaf doesn't exist yet !! " << std::endl ;
                   
                 //       return NULL ;
          }

 //       }

     }


        inline struct Node* rvalGetLeaf(int i){
	
	
	  if( i <= this->children_Num ){	 

            if(this->right && i == 0) return this->right   ;
	    else if(this->left && i == 1) return this->left   ;
	    else if(this->topright) return this->topright ;
	    else if(this->bottomright && i == IsOtherLeafexist(this->bottomright))  return this->bottomright ;
	    else if(this->topleft && i == IsOtherLeafexist(this->topleft)) 	    return this->topleft ;
	    else if(this->bottomleft && i == IsOtherLeafexist(this->bottomleft))    return this->bottomleft ; 
	 //   else  return NULL ;		
  	     
	  }else{
		
	    std::cerr << "__FILE__" << ":" << "__LINE__" <<  " This Leaf doesn't exist yet !! " << std::endl ;
	    		return NULL ;	
   	  }
	  
     }

      template <typename Vector, typename DomainInfo> Vector getMissingLeafs(Vector nodes, DomainInfo domain){
		
	 	 Vector leafs  ;     
		
		 Point<dim,T> _pos1 , _pos2 ;
      
                  //      _pos1.get(0)= this->_pos + domain.getDist(this->_lvl + 1) << 1 ;
			_pos1.get(0)= this->_pos + domain.getLvlDist(this->_lvl + 1)  ;
                        _pos1.get(1)= this->_pos - domain.getLvlDist(this->_lvl + 1)  ;

                        _pos2.get(0)= this->_pos - domain.getLvlDist(this->_lvl + 1)  ;
                        _pos2.get(1)= this->_pos + domain.getLvlDist(this->_lvl + 1)  ;


		       Vector vn = {this->topright= new struct Node<T,dim>( this->_pos + domain.getLvlDist( this->_lvl + 1) , EMPTY, this->_lvl + 1) ,
                                      this->bottomright= new struct Node<T,dim>(_pos2, EMPTY, this->_lvl + 1) ,
                                      this->topleft= new struct Node<T,dim>(_pos1, EMPTY, this->_lvl + 1),
                                      this->bottomleft= new struct Node<T,dim>( this->_pos - domain.getLvlDist(this->_lvl + 1), EMPTY, this->_lvl + 1) } ;
		

		 if(this->_lvl == 0){

                         Vector vn = {this->right=new struct Node<T,dim>( this->_pos + domain.getLvlDist(this->_lvl + 1) , EMPTY, this->_lvl + 1) , 
			 	      this->left=new struct Node<T,dim>( this->_pos - domain.getLvlDist(this->_lvl + 1) , EMPTY, this->_lvl + 1) } ;
                  }
		 	

               	          for(auto It = nodes.begin() ; It < nodes.end(); It++){

         		       	  auto it = find(vn.begin(), vn.end(), It) ;
		 
				  vn.erase(it) ;
        	        	  vn.shrink_to_fit() ;
          
	  	 	 }
          
	  		 for(auto It = vn.begin() ; It < vn.end(); It++){
                            
	   	  		 leafs.push_back(It) ;
                
			 }

          		return leafs ;     	
                }
               

     template <typename St> bool operator>(struct Node<St, dim>* Otherchild ){	
	
	 if(this->_lvl == 0){	 
	
		if ( this == Otherchild->parent->right && Otherchild  == Otherchild->parent->left ){     ; 
	
 		       }else{ 

			return false ;
		    }	

	  }else{
		for(int i = 2; i < 5 ;i++){

	                if ( this == Otherchild->parent->topright && Otherchild == Otherchild->parent->GetLeaf(i) ){
			       		
					return true  ;
			    }
		  

			if ( this == Otherchild->bottomright && Otherchild == Otherchild->parent->GetLeaf(i) && Otherchild != Otherchild->parent->GetLeaf(2) ){

                                        return true  ;
	 		    }			
		}

		 if ( this == Otherchild->topleft || Otherchild == Otherchild->parent->GetLeaf(4) || Otherchild == Otherchild->parent->GetLeaf(5)){
				
					return true  ;
					
		}else if ( this == Otherchild->bottomleft && Otherchild == Otherchild->parent->GetLeaf(5)){

                                        return true  ;
		}else{

		   	                return false ;
	       }			
	       
	    }	 

        }	  


    template <typename St> Point<dim,St> signedAdd(Point<dim,St> &dist, signed int sgn1, signed int sgn2, signed int sgn3=0){
		   Point<dim,T> pos ; 	
   		   pos.get(0) = this->_pos.get(0) + sgn1 * dist.get(0)  ;
                   pos.get(1) = this->_pos.get(1) + sgn2 * dist.get(1)  ;
		  if(dim==3){
       		  pos.get(1) = this->_pos.get(1) + sgn3 * dist.get(2)   ;

		 }
		return pos ;		  
   	 }
    
	template <typename St> struct Node<St,dim>* getChild(aggregate<Point<dim,St>, int>& dist){
             //

                struct Node<St, dim> * node ;
		if(dist.get(0) == 1){
		// topright	
                	node->_pos = this->_pos + dist.get(0)  ;

                node->_lvl = this->_lvl + 1 ;
		node->_ID = EMPTY ;
		}else if(dist.get(0) == 2){
		// bottomright
		//	
			node->_pos = signedAdd(dist.get(0),1,-1) ;
		
                	node->_lvl = this->_lvl + 1 ;
  		        node->_ID = EMPTY ;	
		}else if(dist.get(0) == 3){
                // topleft
                //
			node->_pos = signedAdd(dist.get(0),-1,1) ;
               //
                node->_lvl = this->_lvl + 1 ;	
		node->_ID = EMPTY ;
	       	}else if(dist.get(0) == 4){
                // bottomleft
                //
			 node->_pos = signedAdd(dist.get(0),-1,-1) ;
               //
                node->_lvl = this->_lvl + 1 ;
		node->_ID = EMPTY ;
	        }
		return node ;
  	  }



         template <typename St>  Node<St,dim>* Point2Node(Point<dim, St> pos, struct Node<St, dim> * root){

                struct Node<St, dim>* node ;
                node = root ;
                node->_pos = pos ;
                return node ;
         }     


	 template<typename St> Point<2,St> getPointbyDir(Point<2,St> p1, Point<2,St> dist, Point<2,St> lvl_spacing){ 
	 	 	      	 
	         Point<2,St> p2 ;

	     	 p2.get(0) = p1.get(0) + dist.get(0) -  lvl_spacing.get(0) / 2 ;
	     	 p2.get(1) = p1.get(1) + dist.get(1) -  lvl_spacing.get(1) / 2 ;
                 
		 if (dim == 2){
		 	
			return p2 ;

		 }else{
		
		  p2.get(2) = p1.get(2) + dist.get(2) -  lvl_spacing.get(2) / 2 ;			 
		 
		  return  p2 ;
	        }

	  }    
	 

         inline  std::array<struct Node<T, dim>*,dim> FindChildren(struct Node<T, dim>* grandchild, struct TreeInputs::domainInfo<T,dim> &reg ){
                // p1 is the inquired node position , p2 is the partent position.
                // center is the region referece center point.
		// 
                Point<2,T> p1, p2  ;

		Point<3,T> p3      ; 
                
		std::array<struct Node<T,dim>*, dim> child         ;	
	       	
                Point<dim, T> dist = this->_pos - grandchild->_pos ;

                int i = 0 ; // 
                
		std::cout << " there there "  << std::endl ;
	
		 Node<T,dim> * Lvlchild ;

               
		for (int l = this->_lvl; l < grandchild->_lvl;  l++ ){
		                      std::cout << " there -- 1 "  << std::endl ;
		    Lvlchild  = new  Node<T,dim>(getPointbyDir<T>(this->_pos , dist, -1.0 * reg.getLvlDist(l)),0 , l) ;	 
		   
                                      std::cout << " there -- 2 "  << std::endl ;

		    child[i++] = Lvlchild ; 
		}

		                                      std::cout << " there -- 3 "  << std::endl ;
		return child ; 
		

	  }
         
	  
          std::array<struct Node<T, dim>*,dim>   FindParents(struct TreeInputs::domainInfo<T,dim> &reg){
		  
                // p1 is the inquired node position , p2 is the partent position
                // center is the region referece center point.
                 
		      
		 Point<dim,T> dist = reg.center() - this->_pos ; 
                                
		 std::cout <<  " 1  "  << std::endl ;
			

     		 std::array<struct Node<T,dim>*, dim> parent ;

		 std::cout <<  " 2  "  << std::endl ;

                 int i = this->_lvl  ;


		 std::cout <<  " 3  "  << std::endl ;

		struct Node<T,dim> * Lvlparent ;

                 for (int l = this->_lvl; l == 0 ; l-- ){

			 std::cout <<  l  << std::endl ;
                    Lvlparent  = new  Node<T,dim>(getPointbyDir<T>(this->_pos , dist, reg.getLvlDist(l)),0 , l) ;

                    parent[i--] = Lvlparent ;
                
		 }

                 return  parent ;    

	}

	      
	     inline std::string getName(struct Node<T,dim> * root){
		
		   if(root == this->right) return "right" ;
		   if(root == this->left) return "left" ;
	           if(root == this->topright) return "topright" ;
                   if(root == this->bottomright) return "bottomright" ;
	       	   if(root == this->topleft) return "topleft" ;
                   if(root == this->bottomleft) return "bottomleft" ;

	   }	   


	   inline void getNodeInfo(){
	
            std::cout << " Node position: " << this->_pos << std::endl ;
	    std::cout<<  "Node level: " << this->_lvl  << std::endl    ;	
            std::cout<<  "Node ID: " << this->_ID  << std::endl       ;	    
            std::cout << "Number of Child Nodes: " <<  this->childre_Num << std::endl ;
	    std::cout << "=========================== " << std::endl     ;
	    std::cout << "Allocated Children Nodes: " << std::endl     ;  
            std::cout << "-------------------------" << std::endl     ;

	    for(int i = 0 ; i < this->children_Num ; i++){

                std::cout << "Node Number:" << i << " in level: " << rvalGetLeaf(i)->_lvl << "is :" << rvalGetLeaf(i) << std::endl ;
  
	        }

	    std::cout << "=========================== " << std::endl     ;
	                std::cout << " Parent Node position: " << this->parent->_pos << ", parent Node ID: " << this->parent->_ID  << std::endl ;
   	   }
       
       } ;
   } 

#endif /* node_H_ */


/**
 *
 * toqutree (pa3)
 * significant modification of a quadtree .
 * toqutree.cpp
 * This file will be used for grading.
 *
 */

#include "toqutree.h"
#include "stats.h"

toqutree::Node::Node(pair<int,int> ctr, int dim, HSLAPixel a)
	:center(ctr),dimension(dim),avg(a),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
	{}

toqutree::~toqutree(){
	clear(root);
}

toqutree::toqutree(const toqutree & other) {
	root = copy(other.root);
}


toqutree & toqutree::operator=(const toqutree & rhs){
	if (this != &rhs) {
		clear(root);
		root = copy(rhs.root);
	}
	return *this;
}

toqutree::toqutree(PNG & imIn, int k){ 

/* This constructor grabs the 2^k x 2^k sub-image centered */
/* in imIn and uses it to build a quadtree. It may assume  */
/* that imIn is large enough to contain an image of that size. */

/* your code here */
    root = buildTree(&imIn,k);
    stats = new Stats(&imIn);
}

int toqutree::size() {
/* your code here */
    if(this->NW == nullptr && this->NE == nullptr &&
        this->SE == nullptr && this->SW == nullptr) {
        return 1;
    } else {
        int sum = 1;
        if(this->NW != nullptr) {
            sum += size(this->NW);
        }
        if(this->NE != nullptr) {
            sum += size(this->NE);
        }
        if(this->SE != nullptr) {
            sum += size(this->SE);
        }
        if(this->SW != nullptr) {
            sum += size(this->SW);
        }
        return sum;
    }
}


toqutree::Node * toqutree::buildTree(PNG * im, int k) {

/* your code here */

// Note that you will want to practice careful memory use
// In this function. We pass the dynamically allocated image
// via pointer so that it may be released after it is used .
// similarly, at each level of the tree you will want to 
// declare a dynamically allocated stats object, and free it
// once you've used it to choose a split point, and calculate
// an average.
    Node * node;
    if(k == 0){
        pair<int,int> tempctr (0,0);
        pair<int,int> ul (0,0);
        pair<int,int> lr (0,0);
        node = new Node(tempctr,0,stats.getAvg(ul,lr));
        return node;
    }
    if(k == 1){
        pair<int,int> tempctr (0,0);
        pair<int,int> ul (0,0);
        pair<int,int> ur (1,0);
        pair<int,int> ll (0,1);
        pair<int,int> lr (1,1);
        node = new Node(tempctr,1,stats.getAvg(ul,lr));
        node->NW = new Node(ul,0,stats.getAvg(ul,ul));
        node->NE = new Node(ur,0,stats.getAvg(ur,ur));
        node->SE = new Node(lr,0,stats.getAvg(lr,lr));
        node->SW = new Node(ll,0,stats.getAvg(ll,ll));
        return node;
    }
    double minEn = 0;
    pair<int,int> tempctr (0,0);
    for(int i=2**(k-2); i<2**(k-1)+2**(k-2); i++){
        for(int j=2**(k-2); j<2**(k-1)+2**(k-2); j++){
            pair<int,int> ul (i,j);
            pair<int,int> lr (i+2**(k-1)-1,j+2**(k-1)-1);
            if(minEn > stats.entropy(ul,lr)){
                minEn = stats.entropy(ul,lr);
                tempctr.first = i;
                tempctr.second = j;
            }
        }
    }
    pair<int,int> ul = tempctr;
    pair<int,int> ur (tempctr.first+2**(k-1)-1,tempctr.second);
    pair<int,int> lr (tempctr.first+2**(k-1)-1,tempctr.second+2**(k-1)-1);
    pair<int,int> ll (tempctr.first,tempctr.second+2**(k-1)-1);
    node = new Node(tempctr,k,getAvg(ul,lr));
    node->NW = new Node(ul,k-1,stats.getAvg(lr,ul));
    node->NE = new Node(ur,k-1,stats.getAvg(ll,ur));
    node->SE = new Node(lr,k-1,stats.getAvg(ul,lr));
    node->SW = new Node(ll,k-1,stats.getAvg(ur,ll));
    return node;
}

PNG toqutree::render(){

// My algorithm for this problem included a helper function
// that was analogous to Find in a BST, but it navigated the 
// quadtree, instead.

/* your code here */
    PNG png = new PNG;
    return png;
}

/* oops, i left the implementation of this one in the file! */
void toqutree::prune(double tol){

	prune(root,tol);

}

/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){
/* your code here */
    if(curr != NULL) {
        clear(curr->NW);
        clear(curr->NE);
        clear(curr->SE);
        clear(curr->SW);
        curr = NULL;
    }
}

/* done */
/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {
/* your code here */
    root = other;
}

void toqutree::prune(Node * node, double tol) {
	if(node->NW == nullptr && node->NE == nullptr &&
	    node->SW == nullptr && node->SE == nullptr) {
		return;
	}
	if(canPrune(node,node,tol)){
	    clear(node->NW);
	    clear(node->NE);
	    clear(node->SE);
	    clear(node->SW);
	} else {
	    prune(node->NW,tol);
	    prune(node->NE,tol);
	    prune(node->SE,tol);
	    prune(node->SW,tol);
	}
}

bool toqutree::canPrune(Node * rnode, Node * cnode, double tol){
    if(cnode == nullptr) {
        return true;
    }
	if(cnode->NW == nullptr && cnode->NE == nullptr &&
	    cnode->SW == nullptr && cnode->SE == nullptr) {
		return rnode->avg.dist(cnode->avg) <= tol;
	} else {
	    bool cpnw = false;
	    bool cpne = false;
	    bool cpse = false;
	    bool cpsw = false;
		if (cnode->NW != nullptr) {
			cpnw = canPrune(rnode, cnode->NW, tol);
		}
		if (cnode->NE != nullptr) {
			cpnw = canPrune(rnode, cnode->NE, tol);
		}
		if (cnode->SE != nullptr) {
			cpnw = canPrune(rnode, cnode->SE, tol);
		}
		if (cnode->SW != nullptr) {
			cpnw = canPrune(rnode, cnode->SW, tol);
		}
		return cpnw && cpne && cpse && cpsw;
	}
}
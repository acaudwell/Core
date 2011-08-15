/*
    Copyright (c) 2009 Andrew Caudwell (acaudwell@gmail.com)
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
       derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "quadtree.h"

// QUAD NODE

// allow more items in node if
// - has no children and one of any of:
//    * maximum depth reached
//    * item count is less than desired capacity


bool QuadNode::allowMoreItems() {
    return (children.empty() && (depth >= tree->max_node_depth || items.size() < tree->max_node_items ) );
}


void QuadNode::addItem(QuadItem* item) {

    if(allowMoreItems()) {
        tree->item_count++;
        item->node_count++;
        items.push_back(item);
        return;
    }

    if(!children.empty()) {
        addToChild(item);
        return;
    }

    vec2 average = bounds.centre();

    vec2 middle = average - bounds.min;

    vec2 relmax = bounds.max-bounds.min;

    Bounds2D newbounds;

    children.reserve(4);

    //top left
    newbounds = Bounds2D( bounds.min + vec2(0.0, 0.0), bounds.min + middle );
    children.push_back(new QuadNode(tree, this, newbounds, depth));

    //top right
    newbounds = Bounds2D( bounds.min + vec2(middle.x, 0.0), bounds.min + vec2(relmax.x,middle.y) );
    children.push_back(new QuadNode(tree, this, newbounds, depth));

    //bottom left
    newbounds = Bounds2D( bounds.min + vec2(0.0, middle.y), bounds.min + vec2(middle.x,relmax.y) );
    children.push_back(new QuadNode(tree, this, newbounds, depth));

    //bottom right
    newbounds = Bounds2D( bounds.min + middle, bounds.max );
    children.push_back(new QuadNode(tree, this, newbounds, depth));

    for(std::list<QuadItem*>::iterator it = items.begin(); it != items.end(); it++) {
        QuadItem* oi = *it;
        tree->item_count--;
        oi->node_count--;
        addToChild(oi);
    }

    items.clear();

    addToChild(item);
}


void QuadNode::addToChild(QuadItem* item) {
    if(children.empty()) return;

    for(int i=0;i<4;i++) {
        if(children[i]->bounds.overlaps(item->quadItemBounds)) {
            children[i]->addItem(item);
        }
    }
}


void QuadNode::getLeavesInFrustum(std::set<QuadNode*>& nodeset, Frustum& frustum) {

    if(!items.empty()) {
        nodeset.insert(this);
        return;
    }

    if(children.empty()) return;

    //for each 4 corners
    for(int i=0;i<4;i++) {
        if(!children[i]->empty() && frustum.intersects(children[i]->bounds)) {
            children[i]->getLeavesInFrustum(nodeset, frustum);
        }
    }

    return;
}


int QuadNode::getItemsInFrustum(std::set<QuadItem*>& itemset, Frustum& frustum) {

    if(!items.empty()) {
        int items_added = 0;
        for(std::list<QuadItem*>::iterator it = items.begin(); it != items.end(); it++) {
            QuadItem* oi = (*it);

            if(oi!=0) {
                itemset.insert(oi);
                items_added++;
            }
        }
        return items_added;
    }

    if(children.empty()) return 0;

    int count = 0;

    //for each 4 corners
    for(int i=0;i<4;i++) {
        if(!children[i]->empty() && frustum.intersects(children[i]->bounds)) {
            count += children[i]->getItemsInFrustum(itemset, frustum);
        }
    }

    return count;
}


int QuadNode::getItemsInBounds(std::set<QuadItem*>& itemset, Bounds2D& bounds) const{

    if(!items.empty()) {
        int items_added = 0;

        for(std::list<QuadItem*>::const_iterator it = items.begin(); it != items.end(); it++) {
            QuadItem* oi = (*it);
            itemset.insert(oi);
            items_added++;
        }

        return items_added;
    }

    if(children.empty()) return 0;

    int count = 0;

    //for each 4 corners
    for(int i=0;i<4;i++) {
        if(!children[i]->empty() && bounds.overlaps(children[i]->bounds)) {
            count += children[i]->getItemsInBounds(itemset, bounds);
        }
    }

    return count;
}




int QuadNode::getItemsAt(std::set<QuadItem*>& itemset, vec2 pos) {

    if(!items.empty()) {
        int items_added = 0;
        for(std::list<QuadItem*>::iterator it = items.begin(); it != items.end(); it++) {
            QuadItem* oi = (*it);
            if(oi!=0) {
                itemset.insert(oi);
                items_added++;
            }
        }
        return items_added;
    }

    if(children.empty()) return 0;

    int index = getChildIndex(pos);

    if(index == -1) return 0;

    return children[index]->getItemsAt(itemset, pos);
}

void QuadNode::visitLeavesInFrustum(const Frustum& frustum, VisitFunctor<QuadNode> & visit){

    if(!items.empty()) {

        visit(this);

    }else if(!children.empty()){

      //visit each corner
      for(int i=0;i<4;i++)
        if(!children[i]->empty() && frustum.intersects(children[i]->bounds))
            children[i]->visitLeavesInFrustum(frustum, visit);

    }

}


void QuadNode::visitItemsInFrustum(const Frustum & frustum, VisitFunctor<QuadItem> & visit){

    if(!items.empty()) {

        for(std::list<QuadItem*>::const_iterator it = items.begin(); it != items.end(); it++)
            visit(*it);

    }else if(!children.empty()){

        //visit each corner
        for(int i=0;i<4;i++)
          if(!children[i]->empty() && frustum.intersects(children[i]->bounds))
            children[i]->visitItemsInFrustum(frustum, visit);

    }

}


void QuadNode::visitItemsInBounds(const Bounds2D & bounds, VisitFunctor<QuadItem> & visit){

    if(!items.empty()) {

        for(std::list<QuadItem*>::const_iterator it = items.begin(); it != items.end(); it++)
            visit(*it);

    }else if(!children.empty()){

      //visit each corner
      for(int i=0;i<4;i++)
        if(!children[i]->empty() && bounds.overlaps(children[i]->bounds))
            children[i]->visitItemsInBounds(bounds, visit);

    }

}

void QuadNode::visitItemsAt(const vec2 & pos, VisitFunctor<QuadItem> & visit){

  if(!items.empty()){

    for(std::list<QuadItem*>::const_iterator it = items.begin(); it != items.end(); it++)
      if(*it) visit(*it);

  }else if(!children.empty()){

    int index = getChildIndex(pos);
    if(index != -1) children[index]->visitItemsAt(pos, visit);

  }

}

bool QuadNode::empty() {
    return (items.empty() && children.empty());
}


int QuadNode::getChildIndex(const vec2 & pos) const{

    if(children.empty()) return -1;

    for(int i=0;i<4;i++) {
        if(children[i]->bounds.contains(pos)) {
            return i;
        }
    }

    return -1;
}



QuadNode::QuadNode(QuadTree* tree, QuadNode* parent, Bounds2D bounds, int parent_depth) {

    this->parent = parent;
    this->tree   = tree;
    this->bounds = bounds;
    this->depth  = parent_depth + 1;

    listid = 0;

    tree->node_count++;
}


QuadNode::~QuadNode() {

    if(listid) glDeleteLists(listid, 1);

    if(!children.empty()) {
        for(int i=0;i<4;i++) {
            delete children[i];
        }
    }

    tree->item_count -= items.size();

    items.clear();

    tree->node_count--;
}


int QuadNode::usedChildren() {
    int populated = 0;

    if(!children.empty()) {
        for(int i=0;i<4;i++) {
            if(!children[i]->empty()) populated++;
        }
    }

    return populated;
}


int QuadNode::draw(Frustum& frustum) {

    if(listid && !items.empty()) {
        glPushMatrix();
            glCallList(listid);
        glPopMatrix();
        return 1;
    }

    int drawn = 0;

    if(!children.empty()) {
        for(int i=0;i<4;i++) {
            QuadNode* c = children[i];
            if(!c->empty() && frustum.intersects(c->bounds)) {
                drawn += c->draw(frustum);
            }
        }
    }

    return drawn;
}


void QuadNode::generateLists() {

    if(!items.empty()) {
        if(!listid) listid = glGenLists(1);

        glNewList(listid, GL_COMPILE);

        for(std::list<QuadItem*>::iterator it = items.begin(); it != items.end(); it++) {
            QuadItem* oi = (*it);
            oi->drawQuadItem();
        }

        glEndList();
        return;
    }

    if(!children.empty()) {
        for(int i=0;i<4;i++) {
            QuadNode* c = children[i];
            if(!c->empty()) {
                c->generateLists();
            }
        }
    }
}


void QuadNode::outline() {
    //bounds.draw();

    if(!items.empty()) {
        bounds.draw();
        /*glBegin(GL_LINES);
            glVertex2fv(bounds.min);
            glVertex2fv(bounds.max);
        glEnd();*/
    }

    if(children.empty()) return;

    for(int i=0;i<4;i++) {
        QuadNode* c = children[i];
        if(c!=0) {
            c->outline();
        }
    }
}

void QuadNode::outlineItems() {
    if(items.empty() && children.empty()) return;

    for(std::list<QuadItem*>::iterator it = items.begin(); it != items.end(); it++) {
        QuadItem* oi = (*it);
        oi->quadItemBounds.draw();
    }

    if(children.empty()) return;

    for(int i=0;i<4;i++) {
        QuadNode* c = children[i];
        if(c!=0) {
            c->outlineItems();
        }
    }
}
//Quad TREE


QuadTree::QuadTree(Bounds2D bounds, int max_node_depth, int max_node_items) {
    item_count        = 0;
    node_count        = 0;
    unique_item_count = 0;

    this->max_node_depth = max_node_depth;
    this->max_node_items = max_node_items;

    root = new QuadNode(this, 0, bounds, 0);
}


QuadTree::~QuadTree() {
    delete root;
}


int QuadTree::getItemsAt(std::set<QuadItem*>& itemset, vec2 pos) {
    int return_count = root->getItemsAt(itemset, pos);

    return return_count;
}

int QuadTree::getItemsInFrustum(std::set<QuadItem*>& itemset, Frustum& frustum) {
    return root->getItemsInFrustum(itemset, frustum);
}


int QuadTree::getItemsInBounds(std::set<QuadItem*>& itemset, Bounds2D& bounds) const{
    return root->getItemsInBounds(itemset, bounds);
}


void QuadTree::getLeavesInFrustum(std::set<QuadNode*>& nodeset, Frustum& frustum) {
    return root->getLeavesInFrustum(nodeset, frustum);
}


void QuadTree::visitItemsAt(const vec2 & pos, VisitFunctor<QuadItem> & visit){
  return root->visitItemsAt(pos, visit);
}


void QuadTree::visitItemsInFrustum(const Frustum & frustum, VisitFunctor<QuadItem> & visit){
    root->visitItemsInFrustum(frustum, visit);
}


void QuadTree::visitItemsInBounds(const Bounds2D & bounds, VisitFunctor<QuadItem> & visit){
    root->visitItemsInBounds(bounds, visit);
}


void QuadTree::visitLeavesInFrustum(const Frustum& frustum, VisitFunctor<QuadNode> & visit){
    root->visitLeavesInFrustum(frustum, visit);
}


void QuadTree::addItem(QuadItem* item) {
    item->node_count = 0;
    root->addItem(item);
    unique_item_count++;
}


int QuadTree::drawNodesInFrustum(Frustum& frustum) {
    return root->draw(frustum);
}


void QuadTree::generateLists() {
    root->generateLists();
}


void QuadTree::outline() {
    root->outline();
}

void QuadTree::outlineItems() {
    root->outlineItems();
}

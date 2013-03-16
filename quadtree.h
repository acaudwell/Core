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

#ifndef QUADTREE_H
#define QUADTREE_H

#include <set>
#include <list>

#include "gl.h"
#include "bounds.h"
#include "frustum.h"

class QuadItem {
public:
    Bounds2D quadItemBounds;
    int node_count;
    virtual ~QuadItem() {};
    virtual void updateQuadItemBounds() {};
    virtual void drawQuadItem() {};
};

template <class Data>
class VisitFunctor{
  public:
    virtual void operator()(Data *)=0;
};

class QuadTree;

class QuadNode {
    GLuint listid;

    Bounds2D bounds;
    
    std::vector<QuadNode*> children;
    std::list<QuadItem*> items;

    QuadTree* tree;

    int getChildIndex(const vec2 & pos) const;
    void addToChild(QuadItem* item);

    int depth;

    QuadNode* parent;
public:
    bool allowMoreItems();
    int usedChildren();

    QuadNode(QuadTree* tree, QuadNode* parent, Bounds2D itembounds, int parent_depth);
    ~QuadNode();

    void addItem(QuadItem* item); //if not subdivided, subdivide, add to correct subdivided node.

    int getItemsAt(std::set<QuadItem*>& itemset, vec2 pos);
    void getLeavesInFrustum(std::set<QuadNode*>& nodeset, Frustum& frustum);
    int getItemsInFrustum(std::set<QuadItem*>& itemset, Frustum& frustum);
    int getItemsInBounds(std::set<QuadItem*>& itemset, Bounds2D& bounds) const;

    void visitItemsInFrustum(const Frustum & frustum, VisitFunctor<QuadItem> & visit);
    void visitItemsInBounds(const Bounds2D & bounds, VisitFunctor<QuadItem> & visit);
    void visitItemsAt(const vec2 & pos, VisitFunctor<QuadItem> & visit);
    void visitLeavesInFrustum(const Frustum & frustum, VisitFunctor<QuadNode> & visit);

    bool empty();
    void generateLists();
    int draw(Frustum& frustum);
    void outline();
    void outlineItems();
};


class QuadTree {
    Bounds2D bounds;
    QuadNode* root;
public:
    int unique_item_count;
    int item_count;
    int node_count;
    int max_node_depth;
    int max_node_items;

    int getItemsAt(std::set<QuadItem*>& itemset, vec2 pos);
    void getLeavesInFrustum(std::set<QuadNode*>& nodeset, Frustum& frustum);
    int getItemsInFrustum(std::set<QuadItem*>& itemset, Frustum& frustum);
    int getItemsInBounds(std::set<QuadItem*>& itemset, Bounds2D& bounds) const;

    void visitItemsAt(const vec2 & pos, VisitFunctor<QuadItem> & visit);
    void visitLeavesInFrustum(const Frustum & frustum, VisitFunctor<QuadNode> & visit);
    void visitItemsInFrustum(const Frustum & frustum, VisitFunctor<QuadItem> & visit);
    void visitItemsInBounds(const Bounds2D & bounds, VisitFunctor<QuadItem> & visit);
    void addItem(QuadItem* item);
    void generateLists();
    int drawNodesInFrustum(Frustum& frustum);
    QuadTree(Bounds2D bounds, int max_node_depth, int max_node_items);
    ~QuadTree();
    void outline();
    void outlineItems();
};

#endif

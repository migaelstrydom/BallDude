/**
 * Copyright 2013 Migael Strydom
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
 **/
//---------------------------------------------------------------------------
#ifndef NodeH
#define NodeH
//---------------------------------------------------------------------------
#ifndef NULL
    #define NULL 0L
#endif
//---------------------------------------------------------------------------
class BDNode {
 public:
  BDNode() {
    previous = next = this;
    child = parent = NULL;
  }

  virtual ~BDNode() {
    Detach();
    
    while(child) {
      delete child;
    }
  }

  // Relative nodes
  BDNode *parent, *child, *previous, *next;
  
  bool HasChild() { 
    return child != NULL; 
  }

  bool HasParent() { 
    return parent != NULL; 
  }

  bool IsFirstChild() {
    if (HasParent())
      return parent->child == this;
    else
      return false;
  }

  bool IsLastChild() {
    if (HasParent())
      return parent->child->next == this;
    else
      return false;
  }

  //ORDER
  //                parent
  //           /        |         |
  //      3rdChild <- 2ndChild <- firstChild -> 3rdChild
  //                |           |             |
  //             previous    previous      next
  void AttachBehind(BDNode *newChild) {
    if (newChild->HasParent())
      newChild->Detach();

    newChild->parent = this;

    if (child) {
      newChild->next = child->next;
      newChild->previous = child;
      child->next->previous = newChild;
      child->next = newChild;
    } else {
      child = newChild;
    }
  }


  // Attach a child node at the front of this node's list of children
  void AttachInFront(BDNode *newChild) {
    if (newChild->HasParent())
      newChild->Detach();
    
    newChild->parent = this;
		
    if (child) {
      newChild->next = child;
      newChild->previous = child->previous;
      child->previous->next = newChild;
      child->previous = newChild;
    } else {
      child = newChild;
    }
  }

  // Remove this node from its parent node
  void Detach()	{
    // check if this node is the first child
    if(HasParent() && parent->child == this) {
      if(next != this) {
	parent->child = next;
      } else {
	parent->child = NULL;		
      }
    }

    previous->next = next;
    next->previous = previous;

    previous = this;
    next = this;
  }

  // Returns the number of nodes beneath and including this node. 
  int NumberOfNodes() {
    int num = 1;
    if(child) {
      num += child->next->NumberOfNodes();
    }
    
    if(HasParent() && !IsFirstChild()) {
      num += next->NumberOfNodes();
    }
    
    return num;
  }
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

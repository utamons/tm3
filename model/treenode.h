#pragma once

#include <QList>
#include <QtGlobal>

template <class T> class TreeNode
{
public:
    TreeNode(int id, T data) {
        this->id = id;
        this->_data = data;
        parent = nullptr;
    }

    TreeNode() {
        id = 0;
        parent = nullptr;
    }

    ~TreeNode() {
        while (!children.isEmpty())
            delete children.takeFirst();
    }

    int row() const {
        int row = 0;
        Q_ASSERT_X(!isRoot(),"calculate row","node is root and has not row!");
        for (auto node : parent->children) {
            if (node == this)
                break;
            else
                ++row;
        }
        return row;
    }

    void addChild(TreeNode <T> *child) {
        child->parent = this;
        children.append(child);
    }

    void removeChild(TreeNode <T> *child) {
        if (children.removeOne(child)) {
            child->parent = nullptr;
        }
    }

    void clearChildren() {
        children.clear();
    }

    static void moveChildren(TreeNode<T> *dest, TreeNode<T> *src) {
        for ( auto child : src->children ) {
            src->removeChild(child);
            dest->addChild(child);
        }
    }

    TreeNode <T> *childAt(int idx) {
        return children[idx];
    }

    int getParentId() const { return parent->id; }

    int getId() const { return id; }

    T data() const { return _data; }
    void setData(T data) {this->_data=data;}
    void setId(int id) {this->id = id;}

    bool isExpanded() const { return _isExpanded; }
    void setExpanded( bool exp ) { this->_isExpanded = exp; }

     bool hasChildren() const {return children.length() > 0;}

     int childrenCount() const { return children.length(); }

     TreeNode<T> * getParent() { return parent; }

     bool isRoot() const {
         return parent == nullptr;
     }

     TreeNode<T> * findById(int id) {

         TreeNode<T> * result = nullptr;

         if (this->id == id) {
             result = this;
         } else {
             for (TreeNode<T> * child: children){
                 if ((result = child->findById(id)) != nullptr)
                     break;
             }
         }

         return result;
     }

private:

    int id;
    TreeNode<T> *parent;
    QList< TreeNode <T> * > children;
    T _data;
    bool _isExpanded;

};

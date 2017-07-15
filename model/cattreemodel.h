#pragma once

#include <QAbstractItemModel>
#include "category.h"
#include "treenode.h"
#include <QDebug>
#include "dbhelper.h"

struct DeleteNodeResult {
    QModelIndex index;
};

class CatTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit CatTreeModel(QObject *parent = 0);

    ~CatTreeModel() {
        delete root;
        while (!zombies.isEmpty())
            delete zombies.takeFirst();
    }


    QVariant data(const QModelIndex &index, int role) const override;

    QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    inline int columnCount(const QModelIndex&) const override {
        return 1;
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int, int, const QModelIndex &parent) override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;

    QStringList mimeTypes() const override;

    Qt::DropActions supportedDropActions() const override {return Qt::MoveAction;}


    int addNode(Category cat, const QModelIndex& index);

    QModelIndex deleteNode(const QModelIndex& index);

    void updateNode(Category cat, const QModelIndex& index);

    void expandNode(const QModelIndex& index, bool expand);

    Category categoryFromIndex(const QModelIndex& index);

    bool isExpanded(const QModelIndex& index) {
        auto node = nodeFromIndex(index);
        return node->hasChildren() && node->isExpanded();
    }

    QModelIndex getIndex(Category cat);
    static void fillData(Category &cat);

signals:

    void dropped(QModelIndex);
    //void updated();

private:

     QModelIndex getIndex(TreeNode<Category> *node);
      TreeNode<Category>* nodeFromIndex( const QModelIndex& index ) const;

    TreeNode<Category> *root;
    void buildBranch(TreeNode<Category> *parent);
    bool moveCat(TreeNode<Category> *movedCat, const QModelIndex& parentIdx);
    bool scaryQuestion(Category movedCat, Category movedTo);
    void joinCat(TreeNode<Category> *movedCat, const QModelIndex& parentIdx);

    QList < TreeNode<Category>* > zombies;
};

#ifndef CALIGNEDMODEL_H
#define CALIGNEDMODEL_H

#include <QSharedPointer>
#include <QStandardItemModel>

using AlignColumns = QHash<int, int>;
class CAlignedModel : public QStandardItemModel {
    Q_OBJECT
public:
    explicit CAlignedModel(QObject *parent = 0);

    inline void SetAlignColumn(const AlignColumns &align_column)
    {
        m_align_column = align_column;
    }

    virtual QVariant data(const QModelIndex &index,
                          int role = Qt::DisplayRole) const;

private:
    AlignColumns m_align_column;
};

using ModelPtr = QSharedPointer<CAlignedModel>;
#endif  // CALIGNEDMODEL_H

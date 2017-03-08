#include "attached_files_model.h"

#include <QFileInfo>
#include <QList>
#include <QMimeData>
#include <QUrl>

AttachedFilesModel::AttachedFilesModel(QObject* parent)
  : QAbstractListModel(parent)
{
}

void AttachedFilesModel::attach(const QString& filename)
{
  const QModelIndex parent;
  const int newRow = attachments_.size();
  beginInsertRows(parent, newRow, newRow);
  attachments_ << filename;
  endInsertRows();
}

Qt::ItemFlags AttachedFilesModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractListModel::flags(index);
    if (index.isValid())
        f |= Qt::ItemIsDragEnabled;
    return f;
}

int AttachedFilesModel::rowCount(const QModelIndex&) const
{
  return attachments_.size();
}

QVariant AttachedFilesModel::data(const QModelIndex & index, int role) const
{
  if (role == Qt::DisplayRole) {
    QFileInfo fi (attachments_.at(index.row()));
    return fi.fileName();
  }
  return QVariant();
}

QStringList AttachedFilesModel::mimeTypes() const
{
    QStringList types;
    types << "text/uri-list";
    return types;
}

QMimeData* AttachedFilesModel::mimeData(const QModelIndexList& indexes) const
{
  QList<QUrl> urls;
  foreach (const QModelIndex &index, indexes) {
    if (index.isValid()) {
      urls << QUrl::fromLocalFile(attachments_.at(index.row()));
    }
  }

  QMimeData *mimeData = new QMimeData();
  mimeData->setUrls(urls);
  return mimeData;
}

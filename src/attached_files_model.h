#ifndef ATTACHED_FILES_MODEL_H
#define ATTACHED_FILES_MODEL_H

#include <QAbstractListModel>
#include <QStringList>

class AttachedFilesModel : public QAbstractListModel
{
public:
  AttachedFilesModel(QObject* parent=0);

  void attach(const QString& filename);
  const QStringList& attachments() const
    { return attachments_; }

  Qt::ItemFlags flags(const QModelIndex& index) const;
  int rowCount(const QModelIndex& parent) const;
  QVariant data(const QModelIndex& index, int role) const;

  QStringList mimeTypes() const;
  QMimeData* mimeData(const QModelIndexList& indexes) const;

private:
  QStringList attachments_;
};

#endif // ATTACHED_FILES_MODEL_H

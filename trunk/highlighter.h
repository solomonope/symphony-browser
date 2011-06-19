#ifndef HIGHLIGHTER_H
 #define HIGHLIGHTER_H

 #include <QSyntaxHighlighter>

 #include <QHash>
 #include <QTextCharFormat>

 class QTextDocument;

 class Highlighter : public QSyntaxHighlighter
 {
     Q_OBJECT

 public:
     enum Construct {
         Entity,
         Tag,
         Comment,
         Attribute,
         LastConstruct = Attribute
     };
     Highlighter(QTextDocument *document);
     void setFormatFor(Construct construct, QTextCharFormat &format);
     QTextCharFormat getFormatFor(Construct construct);


 protected:
     enum State {
         Normal = -1,
         InComment,
         InTag,
         InAttribute
     };
     void highlightBlock(const QString &text);

 private:
     QTextCharFormat formats[LastConstruct + 1];
 };

 #endif

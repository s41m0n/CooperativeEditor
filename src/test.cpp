#include <iostream>
#include "client/model/Model.h"

int main(int argc, char **argv) {
  Model model;
  model.setEditorId(1);
  model.localInsert(0, 0, 'c', {false,false,false});
  model.localInsert(0, 1, 'i', {false,false,false});
  model.localInsert(1, 0, 'a', {false,false,false});
  model.localInsert(1, 1, 'o', {false,false,false});
  //Mi aspetto  "ci|ao"
  auto a = model.textify().toStdString();

  model.localInsert(2, 0, 's', {false,false,false});
  model.localInsert(2, 1, 'i', {false,false,false});
  model.localInsert(2, 2, 'm', {false,false,false});
  model.localInsert(2, 3, 'o', {false,false,false});
  //Mi aspetto "ci|ao|simo"
  auto b = model.textify().toStdString();

  model.localInsert(0, 1, 'H', {false,false,false});
  //Mi aspetto "cHi|ao|simo"
  auto c = model.textify().toStdString();


  auto d = model.localErase(0, 0, 0, 1)[0].getChar().toLatin1();
  //Mi aspetto "Hi|ao|simo"
  auto dd = model.textify().toStdString();

  auto e = model.localErase(2, 2, 2, 3)[0].getChar().toLatin1();
  //Mi aspetto "Hi|ao|sio"
  auto ee = model.textify().toStdString();

  auto k = model.localErase(1, 0, 2, 3).size();
  auto kk = model.textify().toStdString();
  //Mi aspetto "Hi|"

  auto s = model.getFileText().size();

  //Simulo un altro client
  Model model2;
  model2.setEditorId(2);
  File file("asdasdasd", model.getFileText());
  model2.setCurrentFile(file);
  //Mi aspetto che il file settato sia "Hi|"
  auto m2f = model2.textify().toStdString();

  //Il client2 scrive e diventa => "Hi|-"
  //Symbol sym = model2.localInsert(0,0, '-', {false,false,false});
  Symbol sym2 = model2.localInsert(0,2, '\n', {false,false,false});
  Symbol sym3 = model2.localInsert(1,0, 'P', {false,false,false});

  auto oi = model2.textify().toStdString();

  //model.remoteInsert(sym);
  model.remoteInsert(sym2);
  model.remoteInsert(sym3);

  //Mi aspetto "-Hi|P"
  auto kkk = model.textify().toStdString();

  auto cc = model.localErase(0, 0, 1, 1);
  //Mi aspetto vuoto ""
  auto pp = model.textify().toStdString();

  for(auto &val: cc)
    model2.remoteErase(val);

  //Mi aspetto vuoto ""
  auto jj = model2.textify().toStdString();

  return 0;

}
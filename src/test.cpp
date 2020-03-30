#include "client/model/Model.h"
#include <iostream>

std::vector<std::vector<std::pair<QChar, std::vector<Identifier>>>> toStandardVector(FileText text) {
  std::vector<std::vector<std::pair<QChar, std::vector<Identifier>>>> out;
  for(auto &val: text) {
    auto tmp = std::vector<std::pair<QChar, std::vector<Identifier>>>();
    for(auto &s: val) {
      auto sl = std::vector<Identifier>();
      for(auto &x : s.getPos()) {
        sl.emplace_back(x);
      }
      tmp.emplace_back(std::make_pair(s.getChar(), sl));
    }
    out.emplace_back(tmp);
  }
  return out;
}

void testLocalInsert(Model &client) {

  client.localInsert(0, 'c', {false, false, false});
  client.localInsert(1, 'i', {false, false, false});
  client.localInsert(2, 'a', {false, false, false});
  client.localInsert(3, 'o', {false, false, false});
  client.localInsert(4, '\n', {false, false, false});
  client.localInsert(5, 's', {false, false, false});
  client.localInsert(6, 'i', {false, false, false});
  client.localInsert(7, 'm', {false, false, false});
  client.localInsert(8, 'o', {false, false, false});

  if (client.textify().toStdString() != "ciao\nsimo") {
    auto msg = "testLocalInsert() FAILED! Expected: `ciao\nsimo` , Got: `" +
                     client.textify().toStdString() + "`";
    throw std::runtime_error(msg);
  } else {
    std::cout << "testLocalInsert() SUCCESS!" << std::endl;
  }
}

void testLocalErase(Model &client) {
  client.localErase(0, 1);
  client.localErase(5, 2);

  if (client.textify().toStdString() != "iao\nso") {
    auto msg = "testLocalErase() FAILED! Expected: `iao\nso` , Got: `" +
                     client.textify().toStdString() + "`";
    throw std::runtime_error(msg);
  } else {
    std::cout << "testLocalErase() SUCCESS!" << std::endl;
  }
}

void testRemoteFileSetting(Model &client1, Model &client2) {
  File file("prova.crdt", client1.getFileText());
  client2.setCurrentFile(file);
  if (client2.getFileName() != "prova.crdt" &&
      client2.textify().toStdString() != "Hi|") {
    auto msg = "testRemoteFileSetting() FAILED! Expected: {`prova.crdt`, `"
              + client1.textify().toStdString() + "`} , Got: {`"
              + client2.getFileName().toStdString() + "`, `"
              + client2.textify().toStdString() +"`}";
    throw std::runtime_error(msg);
  } else {
    std::cout << "testRemoteFileSetting() SUCCESS!" << std::endl;
  }
}

//iao\nso
void testRemoteInsert(Model &client1, Model &client2) {

  Symbol sym1 = client2.localInsert(0, '-', {false, false, false});
  client1.remoteInsert(sym1);

  Symbol sym2 = client2.localInsert(2, '\n', {false, false, false});
  client1.remoteInsert(sym2);

  Symbol sym3 = client2.localInsert(8, 'P', {false, false, false});
  client1.remoteInsert(sym3);

  if (client1.textify().toStdString() != client2.textify().toStdString()) {
    auto msg = "testRemoteInsert() FAILED! Expected: `"
              + client2.textify().toStdString()
              + "` , Got: " + client1.textify().toStdString() + "`";
    throw std::runtime_error(msg);
  } else {
    std::cout << "testRemoteInsert() SUCCESS!" << std::endl;
  }
}

// -i\nao\nsoP
void testRemoteErase(Model &client1, Model &client2) {
  auto deleted = client1.localErase(0, 9);
  for (auto &val : deleted)
    client2.remoteErase(val);

  if (client2.textify().toStdString() != client1.textify().toStdString()) {
    auto msg = "testRemoteInsert() FAILED! Expected: `"
              + client1.textify().toStdString()
              + "` , Got: " + client2.textify().toStdString() + "`";
    throw std::runtime_error(msg);
  } else {
    std::cout << "testRemoteErase() SUCCESS!" << std::endl;
  }
}

void testEmptyFileRemote(Model &client1, Model &client2) {
  auto s = client1.localInsert(0, 'a', {false, false, false});
  client2.remoteInsert(s);

  if(client2.textify() != client1.textify()) {
    auto msg = "testEmptyFileRemote() FAILED! Expected `"
        + client1.textify().toStdString()
        + "` , Got: " + client2.textify().toStdString();
    throw std::runtime_error(msg);
  } else {
    std::cout << "testEmptyFileRemote() SUCCESS!" << std::endl;
  }
}

void runTest(Model &client1, Model &client2, int i) {

  testEmptyFileRemote(client1, client2);
  /*
  testLocalInsert(client1);

  testLocalErase(client1);

  testRemoteFileSetting(client1, client2);

  testRemoteInsert(client1, client2);

  testRemoteErase(client1, client2);
  */
}

int main(int argc, char **argv) {
  Model client1, client2;
  client1.setEditorId(1);
  client2.setEditorId(2);

  for (int i = 0; i < 1000; i++) {
    runTest(client1, client2, i);
    std::cout << "TEST " << i << std::endl;
  }
  return 0;
}
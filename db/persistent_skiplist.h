#ifndef STORAGE_LEVELDB_DB_PERSISTENT_SKIPLIST_H_
#define STORAGE_LEVELDB_DB_PERSISTENT_SKIPLIST_H_

#include <vector>
#include <ctime>
#include "leveldb/comparator.h"
#include "leveldb/iterator.h"
#include "util/persist.h"

namespace leveldb {
class PersistentSkiplist {
 public:
  struct Node;

  explicit PersistentSkiplist(const Comparator* cmp);
  PersistentSkiplist(const Comparator* cmp, Node* first, Node* last, size_t size);
  ~PersistentSkiplist();
  size_t ApproximateMemoryUsage();

  // insert key value, return node holding it
  Node* Insert(const Slice& key, const Slice& value);

  // find node with given key
  Node* Find(const Slice& key);

  // erase nodes in range [first, last]
  void Erase(Node* first, Node* last, size_t compaction_size);

  Node* Head() { return head; }
  Node* Tail() { return tail; }

 private:
  bool Equal(const Slice& a, const Slice& b) const { return (comparator->Compare(a, b) == 0); }
  size_t RandomLevel();
  Node* FindGreaterOrEqual(Slice key);
  Node* MakeNode(Slice key, Slice value, size_t level);

  Node* head;
  Node* tail;;
  const Comparator* comparator;
  static const size_t max_level = 16;
  size_t current_level;
  size_t current_size;
};

struct PersistentSkiplist::Node {
  const std::string key;
  const std::string value;
  const size_t level;
  std::vector<Node*> next;
  std::vector<Node*> prev;

  Node(const Slice& k, const Slice& v, size_t level)
      : key(k.data(), k.size()),
        value(v.data(), v.size()),
        level(level) {
    next.resize(level);
    prev.reserve(level);
    for (auto i = 0; i < level; i++) {
      next.push_back(nullptr);
      prev.push_back(nullptr);
    }
  }

  size_t GetSize() { return key.size() + value.size(); }
};

}

#endif //STORAGE_LEVELDB_DB_PERSISTENT_SKIPLIST_H_
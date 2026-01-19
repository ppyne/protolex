# protolex-corelib — Spécification 0.1

## 0. Objectif

`protolex-corelib` définit un **ensemble minimal mais extensible** de structures de données et d’algorithmes fournis par le runtime protolex.

La corelib :

- **n’ajoute aucune règle au langage**

- n’introduit **aucun mot-clé**

- respecte strictement :
  
  - le gel par défaut
  
  - la mutation contrôlée
  
  - l’absence de global
  
  - l’absence de réflexion

Toutes les structures sont exposées comme **objets ordinaires gelés**.

---

## 1. Principes généraux

### 1.1. Pas de structures intégrées au langage

Protolex **ne définit aucune structure de données intégrée** autre que la table.

Toutes les structures ci-dessous sont :

- fournies par le runtime

- accessibles via import

- implémentées comme objets + fonctions

---

### 1.2. Gel et encapsulation

- les objets représentant les structures sont **gelés**

- l’utilisateur **ne peut jamais muter directement** leurs invariants

- la mutation interne est autorisée **uniquement** à l’intérieur des fonctions de la corelib

Cela garantit :

- sécurité

- cohérence

- absence de corruption de structure

---

### 1.3. Nommage et organisation

La corelib est organisée en **espaces d’objets** :

`ds    // data structures algo  // algorithms`

---

## 2. Structures de données fondamentales (`ds`)

---

## 2.1. List — liste chaînée immuable

### 2.1.1. Nature

- liste persistante

- immuable par défaut

- structure récursive explicite

### 2.1.2. Interface minimale

`ds.List.nil ds.List.cons(head, tail) ds.List.isNil(list) ds.List.head(list) ds.List.tail(list)`

### 2.1.3. Fonctions utilitaires

`ds.List.length(list) ds.List.map(fn, list) ds.List.fold(fn, init, list) ds.List.reverse(list)`

### 2.1.4. Propriétés

- aucune mutation

- partage structurel

- coût mémoire maîtrisé

- parfaitement compatible avec `clone`

---

## 2.2. Array — tableau dynamique encapsulé

### 2.2.1. Nature

- structure séquentielle indexée

- ordre garanti **par la structure**

- mutation interne contrôlée

### 2.2.2. Création

`a = ds.Array.new()`

### 2.2.3. Accès

`ds.Array.length(a) ds.Array.get(a, index) ds.Array.set(a, index, value)`

### 2.2.4. Modification

`ds.Array.push(a, value) ds.Array.pop(a) ds.Array.insert(a, index, value) ds.Array.remove(a, index)`

### 2.2.5. Garanties

- l’objet `a` reste gelé côté utilisateur

- toute mutation est interne à la corelib

- indices hors bornes → exception

---

## 2.3. Map — dictionnaire clé → valeur

### 2.3.1. Nature

- association clé → valeur

- implémentation libre (hash, arbre, etc.)

- indépendance totale de la table protolex

### 2.3.2. Interface

`m = ds.Map.new() ds.Map.put(m, key, value) ds.Map.get(m, key) ds.Map.has(m, key) ds.Map.remove(m, key) ds.Map.size(m)`

---

## 2.4. Set — ensemble mathématique

### 2.4.1. Nature

- ensemble sans doublons

- basé conceptuellement sur Map

### 2.4.2. Interface

`s = ds.Set.new() ds.Set.add(s, value) ds.Set.remove(s, value) ds.Set.contains(s, value) ds.Set.size(s)`

---

## 2.5. Stack / Queue / Deque

Structures séquentielles classiques, interfaces explicites :

### Stack (LIFO)

`st = ds.Stack.new() ds.Stack.push(st, value) ds.Stack.pop(st) ds.Stack.peek(st)`

### Queue (FIFO)

`q = ds.Queue.new() ds.Queue.enqueue(q, value) ds.Queue.dequeue(q)`

---

## 2.6. Tree — arbres

### 2.6.1. Arbre binaire (base)

`t = ds.Tree.empty() ds.Tree.insert(t, value, compare) ds.Tree.contains(t, value, compare) ds.Tree.remove(t, value, compare)`

### 2.6.2. Variantes possibles (optionnelles)

- AVL

- Red-Black

- B-Tree

Chaque variante est exposée comme **objet distinct**.

---

## 2.7. Graph — graphes

### 2.7.1. Nature

- graphe orienté ou non orienté

- représentation interne libre

### 2.7.2. Interface minimale

`g = ds.Graph.new() ds.Graph.addNode(g, node) ds.Graph.addEdge(g, from, to) ds.Graph.neighbors(g, node)`

---

## 3. Algorithmes (`algo`)

Les algorithmes sont **séparés des structures**.

---

## 3.1. Tri (`algo.Sort`)

Chaque algorithme est nommé explicitement.

`algo.Sort.quick(array, compare) algo.Sort.merge(array, compare) algo.Sort.heap(array, compare) algo.Sort.insertion(array, compare)`

- aucun tri implicite

- `compare(a, b)` retourne bool ou ordre

---

## 3.2. Recherche (`algo.Search`)

`algo.Search.linear(array, value) algo.Search.binary(array, value, compare) algo.Search.tree(tree, value, compare)`

---

## 3.3. Parcours et graphes

`algo.Search.bfs(graph, start) algo.Search.dfs(graph, start) algo.Search.dijkstra(graph, start) algo.Search.astar(graph, start, heuristic)`

---

## 4. Erreurs et exceptions

- toute violation d’invariant → `throw`

- aucune valeur spéciale retournée

- aucun fallback silencieux

- les exceptions traversent les appels normalement

---

## 5. Ce que corelib ne fait pas

La corelib **ne fournit pas** :

- concurrence

- parallélisme

- persistance

- sérialisation

- I/O

- gestion mémoire

- typage

Ces aspects relèvent du runtime ou de bibliothèques séparées.

---

## 6. Extension et compatibilité

- toute nouvelle structure doit :
  
  - être un objet ordinaire
  
  - respecter le gel
  
  - expliciter toute mutation

- aucune extension ne doit modifier la spec du langage

---

## 7. Statut

Ce document définit **protolex-corelib 0.1**.  
Il est compatible strictement avec **protolex 0.1** et peut évoluer indépendamment.

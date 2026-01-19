# protolex — Spécification 0.1 (version consolidée)

## 0. Objectif du langage

Protolex est un langage prototype-based minimaliste, sans global implicite ni magie sémantique.  
Il privilégie des **invariants forts**, un **comportement déterministe**, et une **explicitation systématique** des effets (mutation, contrôle de flot, dépendances).

---

## 1. Ontologie générale

### 1.1. Tout est valeur

Protolex définit uniquement des valeurs.

Types primitifs en 0.1 :

- `int` (entier signé 64 bits)

- `float` (double précision)

- `bool`

- `null`

- `string` (immuable)

- `function`

- `table`

Il n’existe aucune distinction conceptuelle entre expression et instruction.

---

### 1.2. Tables

La table est le **seul type composite** du langage.

Caractéristiques :

- associative uniquement

- clés : `string` ou `symbol`

- valeurs : toute valeur protolex

- **non ordonnée**

- aucune garantie d’ordre d’itération

- aucune sémantique de liste ou de séquence

---

## 2. Objets et prototypes

### 2.1. Objet

Un objet est une table.

### 2.2. Prototype

- la clé réservée `proto` désigne le prototype

- `proto` peut valoir :
  
  - un objet
  
  - `null`

- toute autre valeur affectée à `proto` est une erreur fatale

Un objet sans clé `proto` a implicitement `proto = null`.

---

### 2.3. Chaîne de prototypes

- la chaîne est linéaire

- les cycles sont **strictement interdits**

- toute tentative de création de cycle provoque une erreur fatale non interceptable

---

## 3. Lookup

### 3.1. Syntaxe

`obj.key`

### 3.2. Sémantique

- recherche dans `obj`

- sinon dans `obj.proto`

- récursivement jusqu’à `proto = null`

- la première occurrence trouvée est retournée

- aucun effet de bord

- aucune information sur l’origine du slot n’est exposée

### 3.3. Absence

- si la clé n’est trouvée nulle part → `undefined`

- `undefined` n’est **pas** une valeur assignable

- `null` est une valeur valide distincte

---

## 4. Existence locale

### 4.1. Fonction `has`

`has(obj, key)`

- retourne `true` si `key` est un slot **local** de `obj`

- ne parcourt jamais la chaîne de prototypes

Il n’existe aucun mécanisme pour déterminer de quel prototype provient un slot hérité.

---

## 5. Fonctions

### 5.1. Fonctions comme valeurs

- une fonction est une valeur ordinaire

- aucune fonction n’est liée à un objet

- il n’existe aucune notion de méthode au niveau du langage

- aucune variable implicite (`this`, `self`) n’est définie

L’usage d’un paramètre nommé `self` est une **convention**, pas un mot-clé.

---

### 5.2. Appel

`f(x)`

Aucune sémantique implicite n’est attachée à l’appel.

### 5.3. Convention d’appel « méthode »

`obj.f(obj)`

Cette forme est strictement équivalente à :

`(f = obj.f; f(obj))`

---

## 6. Mutation et gel

### 6.1. Gel par défaut

Tout objet est **gelé par défaut**.

Sont interdits sur un objet gelé :

- ajout de slot

- modification de slot

- suppression de slot

Toute tentative provoque une exception.

---

### 6.2. Bloc `mutate`

`mutate obj {     ... }`

- autorise temporairement la mutation de `obj`

- le dégel est **lexical**

- le gel est automatiquement restauré à la sortie du bloc

- aucune mutation permanente n’est possible

---

### 6.3. Masquage et prototypes

- muter un objet ne mute jamais son prototype

- il est autorisé de masquer un slot hérité par un slot local

- le gel s’applique uniquement à l’objet ciblé

---

### 6.4. Suppression de slot

La suppression d’un slot s’effectue par :

`undefine obj.key`

- `undefine` rend le slot absent

- le lookup retombera sur le prototype si présent

- `undefine` est une mutation : autorisé uniquement dans `mutate`

---

## 7. Clonage

### 7.1. Fonction `clone`

`new = clone(proto)`

- crée une table vide

- définit `new.proto = proto`

- ne copie aucun slot

- clonage toujours autorisé

Il n’existe aucun autre mécanisme de construction.

---

## 8. Environnement d’exécution

### 8.1. Absence de global

Protolex ne définit **aucun objet global**.

Toute valeur utilisée par un programme doit provenir :

- d’une liaison lexicale

- d’un argument

- d’un import

- de l’environnement initial fourni par le runtime

---

### 8.2. Objets racines

- le runtime fournit un ensemble d’objets racines

- ces objets sont liés lexicalement

- l’environnement initial est figé

- aucun rebinding n’est autorisé

---

## 9. Import

### 9.1. Principe

`mod = import "module.plx" using env`

- le module est exécuté dans un environnement isolé

- l’environnement est explicitement fourni

- le module retourne un objet

- aucun état global n’est partagé implicitement

---

### 9.2. Exceptions et import

- les exceptions traversent les frontières de module sans transformation

- `import` n’intercepte aucune exception

- toute isolation doit être explicite (`try / catch`)

---

## 10. Exceptions

### 10.1. Principe

- `throw` peut lever **n’importe quelle valeur**

- le langage ne distingue pas erreur / signal / événement

- les exceptions sont un mécanisme général de rupture de flot

---

### 10.2. Syntaxe

`throw value`

`try {     ... } catch e {     ... } catch * {     ... } finally {     ... }`

---

### 10.3. `catch *`

- intercepte toute valeur levée par `throw`

- ne donne pas accès à la valeur

- destiné à la protection et à l’isolation

---

### 10.4. `finally`

- exécuté dans tous les cas

- **ne peut ni `throw` ni `return`**

- toute tentative provoque une erreur fatale

- une exception en cours traverse `finally` intacte

---

### 10.5. Erreurs fatales

Certaines violations des invariants du langage sont **fatales et non interceptables**, notamment :

- cycles de prototypes

- corruption du modèle de mutation

- incohérence de l’environnement d’exécution

- erreurs internes du runtime

---

## 11. Réflexion

Protolex interdit toute réflexion complète sur le graphe d’objets.

Il n’existe aucun mécanisme pour :

- parcourir la chaîne de prototypes

- inspecter l’origine d’un slot

- modifier la stratégie de lookup

- introspecter globalement l’état du programme

---

## 12. Structures de données

Protolex ne définit **aucune structure de données complexe intégrée**.

- les tables sont non ordonnées

- aucune structure de liste n’est intégrée au langage

- les structures séquentielles et algorithmiques sont fournies par bibliothèques

Les bibliothèques peuvent proposer :

- listes

- tableaux dynamiques

- maps

- sets

- arbres

- graphes

- algorithmes de tri et de recherche

Ces structures sont implémentées comme objets ordinaires, avec mutation interne contrôlée.

---

## 13. Mécanismes explicitement absents

Protolex ne définit pas :

- classes

- héritage multiple

- constructeurs

- destructeurs

- initialisation implicite

- nettoyage implicite

- variables implicites (`this`, `self`)

- macros

- métaclasses

- monkey-patching silencieux

---

## 14. Principes directeurs

- pas de global

- pas de magie

- pas de fallback implicite

- invariants forts

- sécurité par défaut

- tout est explicite ou n’existe pas

---

## 15. Statut

Cette spécification définit **protolex 0.1**.  
Elle est conceptuellement close et suffisante pour une implémentation complète.

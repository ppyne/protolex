# protolex — DESIGN

Ce document décrit les **principes de conception**, les **choix assumés** et les
**mécanismes volontairement absents** de protolex.

Il ne s’agit pas d’un tutoriel, mais d’un **document de justification** destiné à
comprendre *pourquoi* protolex est conçu ainsi, et *pourquoi il ne ressemble pas*
à d’autres langages existants.

---

## 1. Objectif fondamental

Protolex vise à fournir un langage :

- minimal
- cohérent
- explicite
- sans magie
- avec des invariants forts

L’objectif n’est **pas** de maximiser la concision syntaxique,
mais de **rendre visibles les choix et les effets** dans le code.

---

## 2. Principe central : tout est valeur

Dans protolex :

- fonctions, tables, nombres, booléens, `null` sont des valeurs
- il n’existe aucune distinction entre expression et instruction
- le langage n’introduit aucun concept “à part”

Conséquence directe :
- aucun mécanisme implicite de cycle de vie
- aucune phase cachée (init, teardown, etc.)

---

## 3. Objets = tables, prototypes explicites

Protolex est **prototype-based**, mais de manière stricte et minimale :

- un objet est une table
- un prototype est une table référencée par la clé `proto`
- le lookup est récursif et déterministe
- les cycles de prototypes sont interdits

Aucun autre mécanisme d’héritage n’existe.

---

## 4. Absence de classes (choix assumé)

Les classes sont volontairement absentes.

Raisons :
- duplication de concepts (classe / instance)
- cycles de vie implicites
- confusion entre structure et comportement
- besoin artificiel de constructeurs

Protolex préfère :
- clonage explicite
- composition
- partage par prototype

---

## 5. Absence de global

Protolex ne définit **aucun objet global**.

Tout accès doit provenir :
- d’une liaison lexicale
- d’un argument
- d’un import explicite
- d’un objet racine fourni par le runtime

Ce choix :
- empêche les dépendances cachées
- facilite l’isolement
- rend les modules réellement testables

---

## 6. Gel par défaut, mutation explicite

Tout objet est **gelé par défaut**.

La mutation :

- est toujours locale
- toujours explicite
- toujours encadrée par `mutate`

```protolex
mutate obj {
    obj.x = 1
}
```

Ce modèle :

- empêche les mutations globales accidentelles
- rend les effets visibles dans le code
- évite le “monkey patching” silencieux

---

## 7. Suppression par absence (`undefine`)

Protolex ne “supprime” pas une valeur :  
il rend un slot **absent**.

```protolex
undefine obj.key
```

Conséquences :

- le lookup peut retomber sur le prototype
- pas de valeur spéciale de type “deleted”
- cohérence avec `undefined`

---

## 8. Fonctions sans liaison implicite

Protolex ne définit :

- ni `this`
- ni `self`
- ni équivalent implicite

Une fonction est une valeur autonome.

Le passage explicite de l’objet :

```protolex
obj.f(obj)
```

est considéré comme une **vertu**, pas une faiblesse :

- pas d’ambiguïté
- pas de capture cachée
- pas de dépendance implicite au contexte

---

## 9. Pas de surcharge d’opérateurs

Protolex **refuse explicitement** tout mécanisme de type `operator` :

- pas de surcharge
- pas de redéfinition
- pas de dispatch implicite

Raisons :

- éviter la magie syntaxique
- éviter les ambiguïtés de résolution
- préserver la lisibilité locale

Toutes les opérations sont des **fonctions explicites** :

```protolex
List.map(f, list)
Sort.quick(array, less)
Map.put(m, k, v)
```

---

## 10. Tables non ordonnées

Les tables protolex sont **fondamentalement non ordonnées**.

Elles ne doivent pas être utilisées comme :

- listes
- tableaux
- séquences

Ce choix :

- évite les dépendances implicites à l’ordre
- force l’usage de structures explicites
- empêche les faux amis conceptuels

---

## 11. Pas de littéraux de collections

Protolex ne définit aucun littéral pour :

- listes
- maps
- sets

La construction de collections est réalisée par la corelib :

```protolex
List.of(1, 2, 3)
Map.of("a", 1, "b", 2)
```

Cela permet :

- une syntaxe explicite
- aucune extension du parseur
- une séparation claire langage / bibliothèque

---

## 12. Corelib : ergonomie sans magie

La corelib fournit :

- structures de données classiques
- algorithmes de tri et de recherche

Mais :

- aucune règle de langage
- aucune mutation utilisateur directe
- aucun opérateur spécial

La mutation interne est **encapsulée** dans la bibliothèque.

---

## 13. Exceptions comme mécanisme général

Protolex utilise les exceptions comme mécanisme **général de rupture de flot** :

- `throw` peut lever n’importe quelle valeur
- `catch *` permet l’isolation
- `finally` est strict et non perturbable

Ce modèle permet :

- la gestion d’erreurs
- la gestion d’événements
- la protection de frontières d’exécution

---

## 14. Réflexion volontairement limitée

Protolex interdit la réflexion complète sur le graphe d’objets.

Impossible de :

- inspecter la chaîne de prototypes
- interroger l’origine d’un slot
- modifier dynamiquement le lookup

Ce choix :

- protège les invariants
- empêche les frameworks intrusifs
- maintient la lisibilité

---

## 15. Ce que protolex n’essaie pas de faire

Protolex n’essaie pas d’être :

- un langage “rapide à écrire”
- un langage à macros
- un langage à métaprogrammation lourde
- un langage universel

Il vise :

- la clarté
- la maintenabilité
- la discipline explicite

---

## 16. Positionnement

Protolex est adapté à :

- des systèmes de taille moyenne à grande
- des bases de code longues à maintenir
- des contextes où la lisibilité prime sur la concision

Il est volontairement exigeant.

---

## 17. Conclusion

Protolex repose sur une idée simple :

> **Ce qui n’est pas explicite n’existe pas.**

Chaque choix de conception vise à rendre le code :

- lisible
- analysable
- maîtrisable

Ce document fait partie intégrante de la spécification.


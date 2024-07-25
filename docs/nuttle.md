
# Nuttle

Nuttle is a spec for a language with no core library. The core lib is whatever the application wants it to be.

## Features

### Strongly, dynamically typed

### Smalltalk/Objective-C like message passing

```objc
[myObject unpack: myState statefully: yes]
```

## Formal stuff

### Lexical grammar

$character \rightarrow \text{Valid ASCII printable characters}$
$digit \rightarrow [\texttt{0}\text{--}\texttt{9}]$
$digits \rightarrow digit+$
$bit \rightarrow \texttt{0}|\texttt{1}$
$bits \rightarrow bit+$
$hexdigit \rightarrow [\texttt{0}\text{--}\texttt{9}\texttt{a}\text{--}\texttt{f}\texttt{A}\text{--}\texttt{F}]$
$hexdigits \rightarrow hexdigit+$
$letter \rightarrow [\texttt{a}\text{--}\texttt{z}\texttt{A}\text{--}\texttt{Z}]$
$id \rightarrow (letter | \texttt{\_})(letter | number | \texttt{\_})*$
$decnumber \rightarrow digits (\texttt{.} digits)?$
$binnumber \rightarrow \texttt{0}(\texttt{B}|\texttt{b})bindigits$
$hexnumber \rightarrow \texttt{0}(\texttt{X}|\texttt{x})hexdigits$
$dquotestring \rightarrow \texttt{"}((character \cap \texttt{\textbackslash}|\texttt{"}) | \texttt{\textbackslash} character)*\texttt{"}$
$dquotestring \rightarrow \texttt{'}((character \cap \texttt{\textbackslash}|\texttt{'}) | \texttt{\textbackslash} character)*\texttt{'}$
$string \rightarrow dquotestring | squotestring$

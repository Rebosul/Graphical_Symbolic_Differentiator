# Graphical Symbolic Differentiator (C++)
A C++ desktop application that parses, symbolically differentiates, and simplifies mathematical expressions while rendering their interactive Abstract Syntax Trees (AST).

Features:
- Parsing & AST Construction: Built a custom tokenizer with implicit multiplication support and a stack-based parser to convert infix expressions into Binary Expression Trees.
- Calculus & Simplification Engine: Implemented recursive algorithms for symbolic differentiation (chain, product, and quotient rules) and algebraic simplification (constant folding and like-term reduction).
- Interactive GUI: Created a 2D interface using graphics.h (WinBGIm) featuring camera panning, successive higher-order derivatives, custom themes, and 4-language support.

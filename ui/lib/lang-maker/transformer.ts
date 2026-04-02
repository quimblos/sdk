import { AST, ASTNode } from "./ast"

type TransformerASTRule = (
    ast: AST
) => void

export class Transformer {

    private rules: TransformerASTRule[] = []

    public rule(rule: TransformerASTRule) {
        this.rules.push(rule)
        return this;
    }

    public transform(ast: AST) {
        for (const rule of this.rules) {
            rule(ast)
        }
    }

}
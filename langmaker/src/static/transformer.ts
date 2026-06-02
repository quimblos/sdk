import { AST, ASTNode } from "../semantics/ast"
import { CSTNode } from "../syntax/cst"

type TransformerRule = (
    ast: AST,
    $: {
        error: (ast: ASTNode, cst: CSTNode, err: string) => void
    }
) => void

export class Transformer {

    private ast_rules: {
        rule: TransformerRule
    }[] = []

    public ast_rule(rule: TransformerRule) {
        this.ast_rules.push({ rule })
        return this;
    }

    public transform(ast: AST) {
        for (const rule of this.ast_rules) {
            rule.rule(ast, {
                error: (node: ASTNode, cst: CSTNode, error: string) => {
                    ast.errors.push({ ast: node, cst, pos: '0', error });
                    cst.errors ??= []
                    cst.errors.push(error);
                }
            })
        }
    }

}
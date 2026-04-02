import { AST, ASTNode } from "./ast"
import { CSTNode } from "./cst"

type LinterRule<
    Root extends ASTNode,
    T extends typeof ASTNode
> = (
    node: InstanceType<T>,
    $: {
        root: Root,
        error: (cst: CSTNode, err: string) => void
    }
) => void

type LinterASTRule = (
    ast: AST,
    $: {
        error: (ast: ASTNode, cst: CSTNode, err: string) => void
    }
) => void

export class Linter<
    Root extends ASTNode = ASTNode
> {

    private ast_rules: {
        rule: LinterASTRule
    }[] = []

    private node_rules: {
        node: { new (...args: any[]): ASTNode },
        rule: LinterRule<any, any>
    }[] = []

    public rule<
        T extends typeof ASTNode
    >(node: T, rule: LinterRule<Root, T>) {
        this.node_rules.push({ node, rule })
        return this;
    }

    public ast_rule<
        T extends typeof ASTNode
    >(rule: LinterASTRule) {
        this.ast_rules.push({ rule })
        return this;
    }

    public lint(ast: AST) {
        for (const rule of this.ast_rules) {
            rule.rule(ast, {
                error: (node: ASTNode, cst: CSTNode, error: string) => {
                    ast.errors.push({ ast: node, cst, pos: '0', error });
                    cst.errors ??= []
                    cst.errors.push(error);
                }
            })
        }
        ast.traverse(node => {
            const node_rules = this.node_rules.filter(rule => rule.node && node instanceof rule.node);
            if (!node_rules.length) return;
            for (const rule of node_rules) {
                rule.rule(node, {
                    root: ast.root,
                    error: (cst: CSTNode, error: string) => {
                        ast.errors.push({ ast: node, cst, pos: '0', error });
                        cst.errors ??= []
                        cst.errors.push(error);
                    }
                })
            }
        }, 'up')
    }

}
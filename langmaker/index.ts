import { AST } from "./src/semantics/ast";
import { Semantics } from "./src/semantics/semantics";
import { Linter } from "./src/static/linter";
import { make_syntax_parser } from "./src/syntax/ebnf";

export type { CSTNode } from "./src/syntax/cst";
export { CST } from "./src/syntax/cst";
export { make_syntax_parser };

export { AST, ASTNode } from "./src/semantics/ast";

export type { Semantics } from "./src/semantics/semantics";
export { SemanticsBuilder } from "./src/semantics/semantics";

export { Linter } from "./src/static/linter";

export type { HTMLStyle } from "./src/format/html";
export { to_html } from "./src/format/html";

export function make_parser(
    syntax: string,
    semantics: Semantics,
    linter?: Linter
) {
    const syntax_parser = make_syntax_parser(syntax);
    const ast = new AST(semantics);

    return (input: string) => {
        const cst = syntax_parser(input);
        ast.update(cst);
        if (linter) {
            linter.lint(ast);
        }
        return ast;
    }
}
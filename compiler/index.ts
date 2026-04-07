import { make_syntax_parser, to_html, AST } from "@quimblos/langmaker";
import { quimblos_syntax } from "./src/lang/syntax";
import { quimblos_semantics } from "./src/lang/semantics";
import { make_quimblos_linter } from "./src/lang/linter";
import { QuimblosCompiler } from "./src/compiler";
import { quimblos_style } from "./src/lang/style";
import { Kernel } from "./src/kernel";
import { QuimblosLinker } from "./src/linker";

export function make_compiler(kernel: Kernel) {
    const syntax_parser = make_syntax_parser(quimblos_syntax);
    const ast = new AST(quimblos_semantics);
    const linter = make_quimblos_linter(kernel);

    return (input: string) => {
        const cst = syntax_parser(input);
        ast.update(cst);
        linter.lint(ast);

        const compiled = QuimblosCompiler.compile(kernel, ast);
        const bytecode = QuimblosLinker.link(compiled.nodes, compiled.code);
        
        const hex = QuimblosLinker.to_hex(bytecode)
        const html = to_html(input, ast, quimblos_style);
        return { ast, ir: compiled.code, hex, html };
    }
}
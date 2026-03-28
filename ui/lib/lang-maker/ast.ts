import { CSTNode } from "./cst";
import { make_syntax_parser } from "./ebnf";
import { Linter } from "./linter";
import { Semantics, SemanticsProp } from "./semantics";
import { Style } from "./styling";

export class ASTNode {
    public kind: string
    public cst: CSTNode
    public children?: () => ASTNode[]
}

export class AST {

    private syntax_parser: (input: string) => CSTNode;
    
    public input!: string;
    public root!: ASTNode;
    
    public errors: {
        ast: ASTNode,
        cst: CSTNode,
        pos: string,
        error: string
    }[] = []

    constructor(
        private grammar: {
            syntax: string,
            semantics: Semantics,
            style?: Style,
            linter?: Linter
        }
    ) {
        this.syntax_parser = make_syntax_parser(this.grammar.syntax);
    }

    public update(input: string) {
        const cst = this.syntax_parser(input);
        
        this.input = input;
        this.root = this.build_node(cst, this.grammar.semantics);
        this.errors = [];

        this.lint();
        
        return this.root;
    }

    public lint() {
        this.grammar.linter?.lint(this);
    }

    public traverse(fn: (node: ASTNode) => boolean|void) {
        const down = (node: ASTNode) => {
            if (!node.children) return;
            for (const child of node.children()) {
                if (!child) continue;
                if (fn(child)) continue;
                down(child);
            }
        }
        down(this.root);
    }

    public traverse_cst(fn: (node: CSTNode) => boolean|void) {
        const down = (node: CSTNode) => {
            if (!node.children) return;
            for (const child of node.children) {
                if (!child) continue;
                if (fn(child)) continue;
                down(child);
            }
        }
        down(this.root.cst);
    }

    public to_html() {
        const style = this.grammar.style;
        
        const down = (node: CSTNode) => {
            const node_style = style?.syntax[node.kind];
            if (node?.errors?.length) {
                return `<span\tclass="ast_error">` + node.text + '</span>';
            }

            let out = '';
            if (node_style) out += `<span\tclass="${node_style}">`;

            if (node.children) {
                for (const child of node.children) {
                    out += down(child);
                }
            }
            else {
                out += node.text;
            }

            if (node_style) out += '</span>';
            return out;
        }
        let raw = down(this.root.cst);

        const suffix = this.input.slice(this.root.cst.end+1);
        if (style)
            raw += `<span\tclass="${style.suffix}">${suffix}</span>`;
        else
            raw += suffix;
        
        return raw.replace(/ /g, '&nbsp;').replace(/\n/g, '<br>');
    }

    public n_lines() {
        let n = 0;
        for (let i = 0; i < this.input.length; i++) {
            if (this.input[i] === '\n') {
                n++;
            }
        }
        return n;
    }

    private build_node(cst: CSTNode, semantics: Semantics, builder?: Semantics[string]): ASTNode {
        builder ??= semantics[cst.kind];
        if (!builder) {
            if (cst.children?.length === 1) {
                return this.build_node(cst.children[0], semantics);
            }
            throw new Error(`CST Node '${cst.kind}' has no semantics declared and more than one child (cannot be assumed to be a union)`)
        }

        const props: Record<string, any> = {}
        for (const prop in builder.props) {
            const arg = builder.props[prop];
            if (typeof arg === 'object') {
                if ('cst' in arg) {
                    props[prop] = this.build_node_prop(cst, semantics, arg)
                }
                else if ('$' in arg) {
                    props[prop] = this.build_node(cst, semantics, arg)
                }
            }
            else {
                props[prop] = arg;
            }
        }

        const obj = new builder.$();
        obj.cst = cst;
        Object.assign(obj, props);

        return obj;
    }

    private build_node_prop (
        cst: CSTNode,
        semantics: Semantics,
        arg: Extract<SemanticsProp,{cst: string}>,
        transform?: (val: string) => any
    ) {
        const c = this.query_cst(cst, arg.cst);
        
        // root #text
        if (!c.kind) {
            if (!c.get_text || c.mod)
                throw new Error(`Invalid argument '${arg}' on node '${cst.kind}'`);
            return arg.transform?.(cst.text) ?? cst.text;
        }
        // 1
        if (!c.mod || c.mod === '+') {
            if (c.results.length == 0) throw new Error(`Child '${arg.cst}' not found on node '${cst.kind}'`);
        }
        
        if (c.mod === '*' || c.mod === '+') {
            if (!c.results.length) return [];
            if (c.get_text)
                return c.results.map(node => 
                    transform?.(node.text) ?? node.text
                );
            else
                return c.results.map(node => this.build_node(node, semantics));
        }
        else {
            if (!c.results.length) return;
            const idx = c.mod.startsWith(':') ? parseInt(c.mod.slice(1)) : 0;
            if (c.get_text)
                return transform?.(c.results[idx].text) ?? c.results[idx].text;
            else
                return this.build_node(c.results[idx], semantics);
        }
    }

    private query_cst(cst: CSTNode, kind_query: string): {
        results: CSTNode[],
        get_text: boolean
        kind?: string
        mod?: string
    } {
        const [_, kind, get_text, mod] = kind_query.match(/(.*?)(#.*?)?([*+?]|:\d+|$)/)!;

        if (!cst.children.length)
            return { results: [], get_text: !!get_text, kind, mod }

        const search = (cst: CSTNode) => {
            const results: CSTNode[] = [];
            for (const child of cst.children) {
                if (child.kind === '_group')
                    results.push(...search(child));
                else if (child.kind === kind)
                    results.push(child);
            }
            return results;
        }
        const results = search(cst);

        return { results, get_text: !!get_text, kind, mod };
    }

}
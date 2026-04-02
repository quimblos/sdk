import { CSTNode } from "../syntax/cst";
import { make_syntax_parser } from "../syntax/ebnf";
import { Semantics, SemanticsProp } from "./semantics";

export class ASTNode {
    public cst!: CSTNode
    public children?: () => (ASTNode|undefined)[]

    public describe(d = 0) {
        let str = '';
        str += `${' '.repeat(d*2)}${this.constructor.name} "${this.cst?.text.trim() ?? ''}"\n`;
        for (const child of this.children?.() ?? []) {
            str += child?.describe(d+1) ?? '';
        }
        return str;
    }
}

export class AST {

    public root!: ASTNode;
    
    public errors: {
        ast: ASTNode,
        cst: CSTNode,
        pos: string,
        error: string
    }[] = []

    constructor(
        private semantics: Semantics
    ) {}

    public update(cst: CSTNode) {
        this.root = this.build_node(cst, this.semantics);
        this.errors = [];
        
        return this.root;
    }

    public traverse(fn: (node: ASTNode) => boolean|void, dir: 'down'|'up' = 'down') {
        const down = (node: ASTNode) => {
            if (!node.children) return;
            for (const child of node.children()) {
                if (!child) continue;
                if (dir === 'up') down(child);
                if (fn(child)) continue;
                if (dir === 'down') down(child);
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

    private build_node(cst: CSTNode, semantics: Semantics, builder?: Semantics[string]): ASTNode {
        builder ??= semantics[cst.kind];
        if (!builder) {
            if (cst.children?.length === 1) {
                return this.build_node(cst.children[0]!, semantics);
            }
            throw new Error(`CST Node '${cst.kind}' has no semantics declared and more than one child (cannot be assumed to be a union)`)
        }

        const props: Record<string, any> = {}
        
        const build_prop = (prop: string, arg: SemanticsProp, merge = false) => {
            let val;
            if ('_options' in arg) {
                for (let i = 0; i < arg._options.length; i++) {
                    try {
                        build_prop(prop, arg._options[i]!, merge);
                        break;
                    }
                    catch (e) {
                        if (i == arg._options.length-1) {
                            if (arg.required) {
                                throw new Error(`Property '${prop}' of node '${cst.kind}' doesn't match any of the options`)
                            }
                        }
                    }
                }
                return;
            }
            else if ('_merge' in arg) {
                props[prop] = [];
                for (let i = 0; i < arg._merge.length; i++) {
                    build_prop(prop, arg._merge[i]!, true);
                }
                return;
            }
            else if ('val' in arg) {
                val = arg.val
            }
            else if ('cst' in arg) {
                val = this.build_node_prop(cst, semantics, arg)
            }
            else if ('_fn' in arg) {
                val = arg._fn(cst)
            }
            else if ('$' in arg) {
                val = this.build_node(cst, semantics, arg)
            }
            if (merge) {
                if (Array.isArray(val)) props[prop].push(...val);
                else props[prop].push(val);
            }
            else {
                props[prop] = val;
            }
        }

        
        for (const prop in builder.props) {
            const arg = builder.props[prop]!;
            build_prop(prop, arg);
        }

        const obj = new builder.$();
        obj.cst = cst;
        Object.assign(obj, props);

        return obj;
    }

    private build_node_prop (
        cst: CSTNode,
        semantics: Semantics,
        arg: Extract<SemanticsProp,{cst: string}>
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
                    arg.transform?.(node.text) ?? node.text
                );
            else
                return c.results.map(node => this.build_node(node, semantics));
        }
        else {
            if (!c.results.length) return;
            const idx = c.mod?.startsWith(':') ? parseInt(c.mod.slice(1)) : 0;
            if (c.get_text)
                return arg.transform?.(c.results[idx]!.text) ?? c.results[idx]!.text;
            else
                return this.build_node(c.results[idx]!, semantics);
        }
    }

    private query_cst(cst: CSTNode, kind_query: string): {
        results: CSTNode[],
        get_text: boolean
        kind?: string | undefined
        mod?: string | undefined
    } {
        const [_, kind, get_text, mod] = kind_query.match(/(.*?)(#.*?)?([*+?]|:\d+|$)/)!;

        if (!cst.children?.length)
            return { results: [], get_text: !!get_text, kind, mod }

        const search = (cst: CSTNode) => {
            const results: CSTNode[] = [];
            for (const child of cst.children ?? []) {
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
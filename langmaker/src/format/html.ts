import { AST } from "../semantics/ast";
import { CSTNode } from "../syntax/cst";

export type HTMLStyle = {
    suffix: string
    syntax: {
        [cst_kind: string]: string
    }
    css: {
        [css_class: string]: {
            [css_prop: string]: string
        }
    }
}

export function to_html(input: string, ast: AST, style: HTMLStyle) {
    
    const down = (node: CSTNode) => {
        const node_style = style?.syntax[node.kind];
        if (node?.errors?.length) {
            return `<span\tclass="ast_error">` + node.text + '</span>';
        }

        let out = '';
        if (node_style) out += `<span\tclass="${node_style}">`;

        if (node.children && node.kind !== 'blank') {
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
    let raw = down(ast.root.cst);

    const suffix = input.slice(ast.root.cst.end+1);
    if (style)
        raw += `<span\tclass="${style.suffix}">${suffix}</span>`;
    else
        raw += suffix;
    
    return raw.replace(/ /g, '&nbsp;').replace(/\n/g, '<br>');
}
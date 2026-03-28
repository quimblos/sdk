import { ASTNode } from "./ast";

type CustomASTNode = { new (...args: any): ASTNode }

type SemanticsTransform = (text: string) => any

export type SemanticsProp = string
                    | { cst: string, transform?: SemanticsTransform }
                    | { $: CustomASTNode, props: Record<string, SemanticsProp> };

export type Semantics = {
    [cst_kind: string]: { $: CustomASTNode, props: Record<string, SemanticsProp> }
}

class SemanticsPropFactory {
    public value(value: string): SemanticsProp {
        return value;
    }
    public first(kind: string): SemanticsProp & { optional: SemanticsProp } {
        return { cst: kind, optional: { cst: kind+'?'} as any };
    }
    public all(kind: string): SemanticsProp & { optional: SemanticsProp }  {
        return { cst: kind+'+', optional: { cst: kind+'*'} as any };
    }
    public nth(kind: string, index: number): SemanticsProp {
        return { cst: kind+':'+index };
    }
    public first_text(kind: string, transform?: SemanticsTransform): SemanticsProp {
        return { cst: kind+'#text', transform };
    }
    public nth_text(kind: string, index: number, transform?: SemanticsTransform): SemanticsProp {
        return { cst: kind+'#text:'+index, transform };
    }
    public text(transform?: SemanticsTransform): SemanticsProp {
        return { cst: '#text', transform };
    }

    public node<T extends CustomASTNode>(_class: T, def: SemanticsPropDef<T>): SemanticsProp {
        return { $: _class, props: def(new SemanticsPropFactory()) }
    }
}

type SemanticsPropDef<
    T extends CustomASTNode
> = (factory: SemanticsPropFactory) => {
    [K in Exclude<keyof InstanceType<T>, 'cst'|'kind'|'children'|'errors'>]: SemanticsProp
}

export class SemanticsBuilder {

    private semantics: Semantics = {}

    public node<T extends CustomASTNode>(kind: string, _class: T, def: SemanticsPropDef<T>) {
        this.semantics[kind] = {
            $: _class,
            props: def(new SemanticsPropFactory())
        };
        return this;
    }

    public build() {
        return this.semantics;
    }

}
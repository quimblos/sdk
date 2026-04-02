export type CSTNode = {
    kind: string,
    start: number,
    end: number,
    text: string,
    children?: CSTNode[]
    errors?: string[]

    tab?: number // Added by linter
}

export class CST {

    public static first(cst: CSTNode, kind: string): CSTNode|undefined {
        for (const child of cst.children) {
            if (child.kind === kind) {
                return child;
            }
            else if (child.kind === '_group') {
                const group_result = this.first(child, kind);
                if (group_result) return group_result;
            }
        }
    }

}
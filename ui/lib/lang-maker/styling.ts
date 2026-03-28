export type Style = {
    suffix: string
    syntax: {
        [kind: string]: string
    }
    css: {
        [css_class: string]: {
            [css_prop: string]: string
        }
    }
}
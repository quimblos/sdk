export function n_lines(text: string) {
    let n = 0;
    for (let i = 0; i < text.length; i++) {
        if (text[i] === '\n') {
            n++;
        }
    }
    return n;
}
import { parse_ebnf, make_syntax_parser } from "../src/syntax/ebnf"

describe('EBNF', () => {
    
    describe('parse_ebnf: basics', () => {

        it('should ignore comment', () => {
            const rules = parse_ebnf('grammar /* comment */ ::= \'\'');
            expect(rules).toEqual({
                'grammar': [['literal','']]
            })
        })

        it('should parse literal', () => {
            const rules = parse_ebnf('grammar ::= \'a\'');
            expect(rules).toEqual({
                'grammar': [['literal', 'a']]
            })
        })

        it('should parse regex', () => {
            const rules = parse_ebnf('grammar ::= [a~z]');
            expect(rules).toEqual({
                'grammar': [['regex', 'a~z']]
            })
        })

        it('should parse or', () => {
            const rules = parse_ebnf('grammar ::= \'a\' | \'b\'');
            expect(rules).toEqual({
                'grammar': [['literal', 'a'], ['or', '|'], ['literal', 'b']]
            })
        })

        it('should parse sequence', () => {
            const rules = parse_ebnf('grammar ::= \'a\' \'b\'');
            expect(rules).toEqual({
                'grammar': [['literal', 'a'], ['literal', 'b']]
            })
        })

        it('should parse group', () => {
            const rules = parse_ebnf('grammar ::= (\'a\' \'b\')');
            expect(rules).toEqual({
                'grammar': [['group', [['literal', 'a'], ['literal', 'b']]]]
            })
        })

        it('should parse rule', () => {
            const rules = parse_ebnf('grammar ::= letter\nletter ::= \'a\'');
            expect(rules).toEqual({
                'grammar': [['rule', 'letter']],
                'letter': [['literal', 'a']]
            })
        })

    })
    
    describe('parse_ebnf: modifiers', () => {

        it('should parse * modifier', () => {
            const rules = parse_ebnf('grammar ::= \'a\'*');
            expect(rules).toEqual({
                'grammar': [['literal', 'a', '*']]
            })
        })

        it('should parse + modifier', () => {
            const rules = parse_ebnf('grammar ::= \'a\'+');
            expect(rules).toEqual({
                'grammar': [['literal', 'a', '+']]
            })
        })

        it('should parse ? modifier', () => {
            const rules = parse_ebnf('grammar ::= \'a\'?');
            expect(rules).toEqual({
                'grammar': [['literal', 'a', '?']]
            })
        })

    })
    
    describe('make_ebnf_parser', () => {

        it('should parse single character syntax', () => {
            const parser = make_syntax_parser('grammar ::= \'a\'');
            const cst = parser('a');
            expect(cst).toEqual({
                kind: 'grammar',
                children: [ { kind: '_literal', t: 0, start: 0, end: 0, text: 'a' } ],
                text: 'a',
                start: 0,
                end: 0
            })
        })

        it('should parse many single character syntax', () => {
            const parser = make_syntax_parser('grammar ::= (\'a\' | \'b\')+');
            const cst = parser('aba');
            expect(cst).toEqual({
                kind: 'grammar',
                children: [
                    { kind: '_group', t: 0, start: 0, end: 0, text: 'a', children: [
                        { kind: '_literal', t: 0, start: 0, end: 0, text: 'a' },
                    ] },
                    { kind: '_group', t: 0, start: 1, end: 1, text: 'b', children: [
                        { kind: '_literal', t: 2, start: 1, end: 1, text: 'b' },
                    ] },
                    { kind: '_group', t: 0, start: 2, end: 2, text: 'a', children: [
                        { kind: '_literal', t: 0, start: 2, end: 2, text: 'a' },
                    ] },
                ],
                text: 'aba',
                start: 0,
                end: 2
            })
        })

    })

})
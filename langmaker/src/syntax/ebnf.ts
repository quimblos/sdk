import { CSTNode } from "./cst";

type EBNFTerm = [string, string | EBNFTerm[]] | [string, string | EBNFTerm, '*'|'+'|'?']

export function make_syntax_parser(ebnf: string) {

  const rules = parse_ebnf(ebnf);

  let fn_str = '';
  fn_str += 'const n = input.length;\n';
  fn_str += 'let i = 0;\n';
  fn_str += 'let last_nl = -1;\n';

  const rules_str = [];

  const make_rule = (name: string, terms: EBNFTerm[]) => {
    let rule_str = '';
    rule_str += `const _${name} = (i) => {\n`;

    rule_str += `  let s = 0, t = [];\n`;
    rule_str += '  while (i<n) {\n';
    rule_str += '    if (input[i] === "\\n") last_nl = i;\n';

    let n = 0;
    for (let i = 0; i < terms.length; i++) {
      const term = terms[i]!;
      if (term[0] === 'or') continue;
      //

      switch (term[0]) {
        case 'regex':
        {
          const re = (term[1] as string).replace(/#/g,'\\');
          rule_str += `    ${i > 0 ? 'else ' : ''}if (s == ${i}) { /* [${term[1]}]${term[2] ?? ''} */\n`;
          rule_str += `      const match = input[i].match(/[${re}]/);\n`
          rule_str += `      if (match) {\n`
          rule_str += `        const node = { kind: '_literal', t: ${i}, start: i, end: i, text: input[i] };\n`;
          rule_str += `        t.push(node); \n`;
          rule_str += `        i += 1;\n`
          break;
        }  
        // case 'or': break;
        case 'rule':
        {
          rule_str += `    ${i > 0 ? 'else ' : ''}if (s == ${i}) { /* ${term[1]}${term[2] ?? ''} */\n`;
          rule_str += `      const start = i;\n`;
          rule_str += `      const children = _${term[1]}(i);\n`
          rule_str += `      if (children?.length) {\n`
          rule_str += `        const end = children.at(-1).end;\n`;
          rule_str += `        t.push({ kind: '${term[1]}', t: ${i}, start, end, children, tab: children.tab, text: input.slice(start, end+1) });\n`;
          rule_str += `        i = end + 1;\n`
          break;
        }  
        case 'literal': {
          const re = (term[1] as string).replace(/\//g,'\\/').replace(/([\*\+\?\[\]\(\)\^\$\.])/g,'\\$1');
          rule_str += `    ${i > 0 ? 'else ' : ''}if (s == ${i}) { /* '${term[1]}'${term[2] ?? ''} */\n`;
          rule_str += `      const match = input.slice(i).match(/^${re}/);\n`
          rule_str += `      if (match) {\n`
          rule_str += `        t.push({ kind: '_literal', t: ${i}, start: i, end: i+match[0].length-1, text: match[0] });\n`;
          rule_str += `        i += match[0].length;\n`
          break;
        }  
        case 'group': {
          rules_str.push(make_rule(`${name}_${i}`, term[1] as EBNFTerm[]));
          rule_str += `    ${i > 0 ? 'else ' : ''}if (s == ${i}) { /* group */\n`;
          rule_str += `      const start = i;\n`;
          rule_str += `      const children = _${name}_${i}(i);\n`
          rule_str += `      if (children?.length) {\n`
          rule_str += `        const end = children.at(-1).end;\n`;
          rule_str += `        t.push({ kind: '_group', t: ${i}, start, end, children, text: input.slice(start, end+1) });\n`;
          rule_str += `        i = end + 1;\n`
          break;
        }
      }
      if (terms[i+1]?.[0] === 'or') {
        rule_str += `        break;` // Next term is a "|", but was already found, so break it
      }
      else if (!term[2]) {
        rule_str += `        s++; continue;\n` // required found, proceed to next step
      }
      else if (term[2] === '*') {
        rule_str += `        continue;\n`     // 0 or + found, proceed on same step
      }
      else if (term[2] === '+') {
        rule_str += `        continue;\n`     // 1 or + found, proceed on same step
      }
      else if (term[2] === '?') {
        rule_str += `        s++; continue;\n` // optional found, proceed to next step
      }
      rule_str += `      }\n`;

      rule_str += `      else {\n`
      // Next term is a "|", so attempt the next option directly
      if (terms[i+1]?.[0] === 'or') {
        rule_str += `        s += 2; continue;`
      }
      // 
      else {
        if (!term[2]) {
          rule_str += `        return;\n` // required not found, stop
        }
        else if (term[2] === '*') {
          rule_str += `        s++; continue;\n` // 0 or + not found, proceed to next step
        }
        else if (term[2] === '+') {
          rule_str += `        if (t.at(-1)?.t !== ${i}) return;\n` // 1 or +, no first, stop
          rule_str += `        s++; continue;\n` // 1 or + found, first ok, proceed to next step
        }
        else if (term[2] === '?') {
          rule_str += `        s++; continue;\n` // optional not found, proceed to next step
        }
      }
      rule_str += `      }\n`
      rule_str += `    }\n`;
      n++;
    }
    if (n) {
      rule_str += '    else { break; }\n';
    }
    rule_str += '  }\n';
    rule_str += `  return t;\n`;
    rule_str += '}\n';
    return rule_str;
  }

  for (const name in rules) {
    const rule = rules[name]!;
    rules_str.push(make_rule(name, rule));
  }

  for (const rule of rules_str) {
    fn_str += rule;
  }

  fn_str += 'const children = _grammar(0) ?? [];\n'
  fn_str += '\n'
  fn_str += 'return {\n'
  fn_str += '  kind: \'grammar\',\n';
  fn_str += '  children,\n';
  fn_str += '  text: input.slice(0, (children.at(-1)?.end ?? 0) + 1),\n';
  fn_str += '  start: 0,\n';
  fn_str += '  end: children.at(-1)?.end ?? -1\n';
  fn_str += '}\n';

  const fn = new Function('input', fn_str);
  return fn as (input: string) => CSTNode;
}

export function parse_ebnf(ebnf: string) {

  const lines = ebnf
    .replace(/\/\*.*?\*\//gs, '')
    .replace(/^\s*\/\/.*$/gm, '')
    .split('\n')
    .map(l => l.trim())
    .filter(l => l);

  function make_rule_entries(l: number, input: string) {
    const line = input.trim();

    let state = undefined;
    let col0 = 0;
    
    let stack: EBNFTerm[] = [
      ['', []]
    ];
    
    for (let col = 0; col < line.length; col++) {
      const ch0 = line[col];
      const ch1 = line[col+1];

      if (!state) {
        col0 = col;
        if (ch0 === '/' && ch1 === '*') {
          state = 'comment';
          col++;
        }
        else if (ch0 === '[') {
          state = 'regex';
        }
        else if (ch0 === '\'') {
          state = 'literal';
        }
        else if (ch0 === '(') {
          const term: EBNFTerm = ['group', []];
          (stack[0]![1] as EBNFTerm[]).push(term);
          stack.unshift(term);
        }
        else if (ch0 === ')') {
          stack.shift();
          if (stack.length == 0) throw new Error(`[${l}:${col}] Invalid EBNF: Too many closing parenthesis ')'`);
        }
        else if (ch0 === '|') {
          (stack[0]![1] as EBNFTerm[]).push(['or', '|']);
        }
        else {
          const match = line.slice(col0).match(/^(\w+)/);
          if (match) {
            (stack[0]![1] as EBNFTerm[]).push(['rule', match[1]!]);
            col += match[1]!.length-1;
          }
        }
      }
      else {
        if (state === 'comment') {
          if (ch0 === '*' && ch1 === '/') {
            col0 = col+2;
            state = undefined;
            continue;
          }
        }
        else if (state === 'regex') {
          if (ch0 === ']') {
            (stack[0]![1] as EBNFTerm[]).push(['regex', line.slice(col0+1, col)]);
            col0 = col+1;
            state = undefined;
          }
        }
        else if (state === 'literal') {
          const is_special = (col > 0 && line[col-1] === '\\');
          if (ch0 === '\'' && !is_special) {
            (stack[0]![1] as EBNFTerm[]).push(['literal', line.slice(col0+1, col).replace(/\\'/,"'")]);
            col0 = col+1;
            state = undefined;
          }
        }
      }
      // Modifiers
      if (['+','*','?'].includes(line[col+1]!)) {
        if (!state) {
          const terms = stack[0]![1] as EBNFTerm[];
          terms[terms.length-1]![2] = line[col+1] as any;
          col++;
        }
      }
    }

    if (stack.length != 1) throw new Error(`[${l}:-1] Invalid EBNF: Unclosed group')'`);
    return stack[0]![1] as EBNFTerm[];
  }

  const rules: Record<string, EBNFTerm[]> = {};
  for (let l = 0; l < lines.length; l++) {
    const line = lines[l]!;
    if (!line.length) continue;

    const match_rule = line.match(/^\s*(\w+)\s*::=(.*)/);
    if (!match_rule) throw new Error(`Invalid EBNF syntax at line ${l}`);

    if (match_rule[1]) {
      const name = match_rule[1];
      if (rules[name]) throw new Error(`Invalid EBNF: duplicate production rule: ${name}`);
      
      const terms = make_rule_entries(l, match_rule[2]!);
      if (!terms.length) throw new Error(`Invalid EBNF: empty production rule: ${name}`);
  
      rules[name] = terms;
    }
  }

  const grammar_rule = rules['grammar'];
  if (!grammar_rule) throw new Error(`Invalid EBNF: missing grammar rule`);
  
  return rules;
}
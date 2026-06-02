import { make_compiler } from '@quimblos/compiler';
import { quimblos_style } from '@quimblos/compiler/src/lang/style';

import { QuimblosEngine, n_lines } from '../lib';
import { LED8Driver } from './drivers/led8.driver'
import { LedStripDriver } from './drivers/ledstrip.driver'
import { Sling2DDriver } from './drivers/sling2d.driver';

import Goo from '@quimblos/goo';
import { GooRouter } from '@quimblos/goo/src/router'

import '@quimblos/goo/components/goo-style.goo'
import '@quimblos/goo/components/goo-app.goo'
import '@quimblos/goo/components/goo-page-error.goo'
import '@quimblos/goo/components/goo-card.goo'
import '@quimblos/goo/components/goo-btn.goo'
import '@quimblos/goo/components/goo-list.goo'
import '@quimblos/goo/components/goo-modal.goo'
import '@quimblos/goo/components/goo-form.goo'
import '@quimblos/goo/components/goo-chart.goo'

import './components/qb-console.goo'
import './components/qb-editor.goo'
import './components/qb-servo.goo'
import './components/qb-water-tank.goo'
import './pages/page-welcome.goo'
import './pages/page-drivers.goo'
import './pages/page-sandbox.goo'
import './my-app.goo'

async function setup() {

  // Quimblos Engine

  const engine = await QuimblosEngine.init();

  (window as any).wasm = engine.kernel;
  (window as any).qb = engine;
  
  // engine.link_driver(new LED8Driver());
  // engine.link_driver(new Sling2DDriver());
  // engine.link_driver(new LedStripDriver());
  
  (window as any).qbstyle = quimblos_style;
  (window as any).qbcompile = make_compiler(engine);
  (window as any).n_lines = n_lines;

  // Goo Routes

  const routes = GooRouter.tree('my-app', $ => $
    .alias('Home')
    .menu($ => [$('Home')])
    .child('welcome', $ => $
      .slot('my-app|page', 'page-welcome')
    )
    .child('drivers', $ => $
      .alias('Drivers')
      .menu($ => [$('Drivers')])
      .slot('my-app|page', 'page-drivers')
    )
    .child('sandbox', $ => $
      .alias('Sandbox')
      .menu($ => [$('Sandbox')])
      .slot('my-app|page', 'page-sandbox')
    )
    .follow_to('welcome')
  )

  // Goo Components

  Goo.init([
    'goo-style',
    'my-app'
  ], routes);

}

void setup();
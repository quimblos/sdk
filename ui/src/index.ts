// import { App } from './components/App';
// import './components/qb-device.wml'
import './components/qb-app.wml'
// import './devices/qb-device-water-tank.wml'

// import './lala'
// import './style.scss'

function component() {
  const app = document.createElement('qb-app');
  return app;
}

document.body.appendChild(component());
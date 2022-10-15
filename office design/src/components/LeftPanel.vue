<script>
import interact from 'interactjs';
import { randomId } from '../helpers/utils';

export default {
  data() {
    return {
      dragObj: null,
      position: { x: 0, y: 0 },
      canvas: null,
      rooms: [
        {
          id: randomId(),
          objects: [
            {
              x: 0,
              y: 0,
              width: 250,
              height: 200,
              style: 'fill: Orange;'
            }
          ]
        }, {
          id: randomId(),
          objects: [
            {
              x: 0,
              y: 0,
              width: 250,
              height: 200,
              style: 'fill: Orange;'
            },
            {
              x: 40,
              y: 40,
              width: 80,
              height: 40,
              style: 'fill: Violet;'
            },
            {
              x: 100,
              y: 100,
              width: 80,
              height: 40,
              style: 'fill: Violet;'
            }
          ]
        }
      ],
      decorations: [
        {
          id: randomId(),
          objects: [
            {
              x: 0,
              y: 0,
              width: 100,
              height: 100,
              style: 'fill: Violet;'
            }
          ]
        },
        {
          id: randomId(),
          objects: [
            {
              x: 0,
              y: 0,
              width: 80,
              height: 40,
              style: 'fill: DarkGreen;'
            }
          ]
        }
      ]
    }
  },
  methods: {
    resetDrag() {
      this.dragObj = null
      this.position.x = this.position.y = 0
    }
  },
  mounted() {
    // listening events
    this.emitter.on('canvaschange', (canvas) => {
      this.canvas = canvas
    })



    const roomsObj = interact('.select-room')
    console.log(roomsObj)
    const self = this
    roomsObj.draggable({
      listeners: {
        start(event) {
          console.log(event.type, event.target, event)
          let id = event.target.getAttribute('id').split('-')[1]
          let room = self.rooms.find(r => r.id === id)
          self.emitter.emit('dragstart', room)
          let dragObj = event.target.cloneNode(true)
          dragObj.style.position = 'absolute'
          dragObj.style.top = event.clientY
          dragObj.style.left = event.clientX
          dragObj.style.fillOpacity = '0.5'
          self.$refs['leftPanel'].appendChild(dragObj)
          self.dragObj = dragObj
          self.position.x = self.position.y = 0
        },
        end(event) {
          console.log("on end:", event)
          self.emitter.emit('dragend', { x: event.clientX, y: event.clientY })
          self.$refs['leftPanel'].removeChild(self.dragObj)
          self.resetDrag()
        },
        move(event) {
          self.position.x += event.dx
          self.position.y += event.dy
          self.emitter.emit('dragmove', self.position)
          self.dragObj.style.transform = `translate(${self.position.x}px, ${self.position.y}px)`

          if (self.canvas) {
            if (event.clientX > self.canvas.x + self.canvas.width - 1) {
              console.log(event.clientX, event.clientY)
              self.emitter.emit('movefloor', { dx: -5, dy: 0 })
            }
            if (event.clientY > self.canvas.y + self.canvas.height - 1) {
              self.emitter.emit('movefloor', { dx: 0, dy: -5 })
            }
            if (event.clientY < self.canvas.y) {
              self.emitter.emit('movefloor', { dx: 0, dy: 5 })
            }

          }
        }
      }
    })
  }
}
</script>
<template>
  <div class="left" ref="leftPanel">
    <p>房间</p>
    <ul>
      <li :key="room.i" v-for="room in rooms">
        <svg class="select-room" :id="'svg-' + room.id" height="200" width="250">
          <g>
            <rect :key="object.i" v-for="object in room.objects" :x="object.x" :y="object.y" :width="object.width"
              :height="object.height" :style="object.style"></rect>
          </g>
        </svg>
      </li>
    </ul>
    <p>装饰物</p>
    <ul>
      <li :key="dec.i" v-for="dec in decorations">
        <svg class="select-decoration" :id="'svg-' + dec.id" height="100" width="100">
          <g>
            <rect :key="object.i" v-for="object in dec.objects" :x="object.x" :y="object.y" :width="object.width"
              :height="object.height" :style="object.style"></rect>
          </g>
        </svg>
      </li>
    </ul>
  </div>
</template>


<style scoped>
.left {
  background-color: lightgray;
  overflow: auto;
}

.left ul {
  list-style-type: none;
  padding-left: 20px;
  padding-right: 20px;
}

.left ul>li {
  margin-bottom: 20px;
}
</style>
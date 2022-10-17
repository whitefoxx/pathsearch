<script>
import interact from 'interactjs';
import { randomId } from '../helpers/utils'

export default {
  data() {
    return {
      newRoom: null,
      movingRoom: null,
      canvas: null,
      floor: {
        x: 0,
        y: 0,
        width: "100%",
        height: "100%"
      },
      rooms: [],
    }
  },
  methods: {
    resizeCanvas() {
      let r = this.$refs['canvas'].getBoundingClientRect()
      this.canvas = { x: r.x, y: r.y, width: r.width, height: r.height }
      this.emitter.emit('canvaschange', { x: r.x, y: r.y, width: r.width, height: r.height })
    },
    initFloor() {
      let r = this.$refs['canvas'].getBoundingClientRect()
      this.floor.width = r.width
      this.floor.height = r.height
    },
    moveFloor(dx, dy) {
      let rect = this.$refs['canvas'].getBoundingClientRect()
      this.floor.x += dx
      this.floor.y += dy
      let dw = 0
      let dh = 0
      let addSize = 1000 // 必须是地板格子的整数倍
      if (this.floor.x > 0) {
        dw = this.floor.x + addSize
        this.floor.width += dw
        this.floor.x = -addSize
      }
      if (this.floor.y > 0) {
        dh = this.floor.y + addSize
        this.floor.height += dh
        this.floor.y = -addSize
      }
      let r = this.floor.x + this.floor.width
      let b = this.floor.y + this.floor.height
      if (r < rect.width) {
        dw = (rect.width - r) + addSize
        this.floor.width += dw
      }
      if (b < rect.height) {
        dh = (rect.height - b) + addSize
        this.floor.height += dh
      }
      this.rooms.forEach((r) => {
        if (!this.movingRoom || r.id != this.movingRoom.id) {
          r.objects.forEach((object) => {
            object.x += dx
            object.y += dy
          })
        }
      })

    }
  },
  mounted() {
    this.resizeCanvas()
    this.initFloor()
    window.addEventListener('resize', this.resizeCanvas)

    // from left panel to the canvas
    this.emitter.on('dragstart', (newRoom) => {
      console.log('on dragstart', newRoom)
      this.newRoom = JSON.parse(JSON.stringify(newRoom))
      this.newRoom.id = randomId()
    })
    this.emitter.on('dragmove', (position) => {
      console.log('on dragmove:', position)
    })
    this.emitter.on('dragend', (position) => {
      this.newRoom.objects.forEach((object) => {
        object.x += (position.x - this.canvas.x)
        object.y += (position.y - this.canvas.y)
      })
      this.rooms.push(this.newRoom)
    })
    this.emitter.on('movefloor', ({ dx, dy }) => {
      console.log('movefloor:', dx, dy)
      this.moveFloor(dx, dy)
    })

    // interaction of canvas
    const canvasInteract = interact('#canvas')
    const self = this
    canvasInteract.draggable({
      listeners: {
        start(event) {
        },
        end(event) {
        },
        move(event) {
          self.moveFloor(event.dx, event.dy)
        }
      }
    })

    // interaction of rooms
    const roomsInteract = interact('.room')
    roomsInteract.draggable({
      listeners: {
        start(event) {
          let id = event.target.getAttribute('id').split('-')[1]
          self.movingRoom = self.rooms.find(r => r.id === id)
        },
        end(event) {
          self.movingRoom = null
        },
        move(event) {
          let rect = self.$refs['canvas'].getBoundingClientRect()
          self.movingRoom.objects.forEach(obj => {
            obj.x += event.dx
            obj.y += event.dy
          })
          if (event.clientX > rect.x + rect.width - 1) {
            self.moveFloor(-5, 0)
          }
          if (event.clientX < rect.x) {
            self.moveFloor(5, 0)
          }
          if (event.clientY > rect.y + rect.height - 1) {
            self.moveFloor(0, -5)
          }
          if (event.clientY < rect.y) {
            self.moveFloor(0, 5)
          }
        }
      }
    })
  }
}
</script>

<template>
  <div class="office" ref="office">
    <svg class="canvas" id="canvas" ref="canvas" width="100%" height="100%" xmlns="http://www.w3.org/2000/svg">
      <defs>
        <pattern id="smallGrid" width="10" height="10" patternUnits="userSpaceOnUse">
          <path d="M 10 0 L 0 0 0 10" fill="none" stroke="lightblue" stroke-width="0.5" />
        </pattern>
        <pattern id="grid" width="100" height="100" patternUnits="userSpaceOnUse">
          <rect width="100" height="100" fill="url(#smallGrid)" />
          <path d="M 100 0 L 0 0 0 100" fill="none" stroke="lightblue" stroke-width="1" />
        </pattern>
      </defs>

      <g>
        <g style>
          <svg :width="floor.width" :height="floor.height" :x="floor.x" :y="floor.y">
            <rect width="100%" height="100%" fill="url(#grid)" />
          </svg>
        </g>

        <g>
          <g :key="room.i" class="room" :id="'room-' + room.id" v-for="room in rooms">
            <rect :key="object.i" class="object" :x="object.x" :y="object.y" :width="object.width"
              :height="object.height" :style="object.style" v-for="object in room.objects"></rect>
          </g>
        </g>
      </g>
    </svg>

  </div>
</template>

<style scoped>
.office {
  flex: 1;
  overflow: hidden;
}
</style>
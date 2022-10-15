<script>
import interact from 'interactjs';
import { randomId } from '../helpers/utils'

export default {
  data() {
    return {
      newRoom: null,
      dragRoom: null,
      floor: {
        offsetX: 0,
        offsetY: 0,
        x: 0,
        y: 0,
        width: "100%",
        height: "100%"
      },
      rooms: [],
    }
  },
  mounted() {
    let r = this.$refs['canvas'].getBoundingClientRect()
    this.floor.width = r.width
    this.floor.height = r.height
    this.floor.offsetX = r.x
    this.floor.offsetY = r.y

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
        object.x += (position.x - this.floor.offsetX + this.floor.x)
        object.y += (position.y - this.floor.offsetY + this.floor.y)
      })
      this.rooms.push(this.newRoom)
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
          let rect = self.$refs['canvas'].getBoundingClientRect()
          self.floor.x += event.dx
          self.floor.y += event.dy
          self.floor.offsetX += event.dx
          self.floor.offsetY += event.dy
          let dw = 0
          let dh = 0
          if (self.floor.x > 0) {
            dw = self.floor.x + 100
            self.floor.width += dw
            self.floor.x -= dw
            self.floor.offsetX -= dw
          }
          if (self.floor.y > 0) {
            dh = self.floor.y + 100
            self.floor.height += dh
            self.floor.y -= dh
            self.floor.offsetY -= dh
          }
          let r = self.floor.x + self.floor.width
          let b = self.floor.y + self.floor.height
          if (r < rect.width) {
            dw = (rect.width - r) + 100
            self.floor.width += dw
          }
          if (b < rect.height) {
            dh = (rect.height - b) + 100
            self.floor.height += dh
          }
          self.rooms.forEach((g) => {
            g.objects.forEach((object) => {
              object.x += event.dx
              object.y += event.dy
            })
          })
        }
      }
    })

    // interaction of rooms
    const roomsInteract = interact('.room')
    roomsInteract.draggable({
      listeners: {
        start(event) {
          let id = event.target.getAttribute('id').split('-')[1]
          self.dragRoom = self.rooms.find(r => r.id === id)
        },
        end(event) {
        },
        move(event) {
          self.dragRoom.objects.forEach(obj => {
            obj.x += event.dx
            obj.y += event.dy
          })
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
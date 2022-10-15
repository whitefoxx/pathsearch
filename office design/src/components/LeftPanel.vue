<script>
import interact from 'interactjs';

export default {
  data() {
    return {
      dragObj: null,
      position: { x: 0, y: 0 },
      groups: [
        {
          id: 0,
          shapes: [
            {
              x: 0,
              y: 0,
              width: 250,
              height: 200,
              style: 'fill: rgb(0, 200, 0);'
            }
          ]
        }, {
          id: 1,
          shapes: [
            {
              x: 0,
              y: 0,
              width: 250,
              height: 200,
              style: 'fill: rgb(0, 200, 0);'
            },
            {
              x: 80,
              y: 40,
              width: 100,
              height: 80,
              style: 'fill: rgb(0, 0, 100);'
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
    const interactObj = interact('.material')
    const self = this
    interactObj.draggable({
      listeners: {
        start(event) {
          console.log(event.type, event.target, event)
          let index = parseInt(event.target.getAttribute('id').split('-')[1])
          self.emitter.emit('dragstart', self.groups[index])
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
        }
      }
    })
  }
}
</script>
<template>
  <div class="left" ref="leftPanel">
    <ul>
      <li :key="group.i" v-for="group in groups">
        <svg class="material" :id="'svg-' + group.id" height="200" width="250">
          <g>
            <rect :key="shape.i" v-for="shape in group.shapes" :x="shape.x" :y="shape.y" :width="shape.width"
              :height="shape.height" :style="shape.style"></rect>
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
  width: 250px;
  height: 200px;
}
</style>
<script>
import interact from 'interactjs';
export default {
  data() {
    return {
      dragGroup: null,
      floor: {
        offsetX: 0,
        offsetY: 0,
        x: 0,
        y: 0,
        width: "100%",
        height: "100%"
      },
      groups: [
        {
          shapes: [
            {
              tag: 'rect',
              x: 0,
              y: 0,
              width: 250,
              height: 200,
              style: 'fill:rgb(200, 0, 255);',
            },
            {
              tag: 'rect',
              x: 300,
              y: 300,
              width: 250,
              height: 200,
              style: 'fill:rgb(0, 0, 255);',
            }
          ]
        }
      ],
    }
  },
  mounted() {
    let r = this.$refs['canvas'].getBoundingClientRect()
    this.floor.width = r.width
    this.floor.height = r.height
    this.floor.offsetX = r.x
    this.floor.offsetY = r.y

    // from left panel to the canvas
    this.emitter.on('dragstart', (dragGroup) => {
      console.log('on dragstart', dragGroup)
      this.dragGroup = JSON.parse(JSON.stringify(dragGroup))
    })
    this.emitter.on('dragmove', (position) => {
      console.log('on dragmove:', position)
    })
    this.emitter.on('dragend', (position) => {
      this.dragGroup.shapes.forEach((shape) => {
        shape.x += (position.x - this.floor.offsetX + this.floor.x)
        shape.y += (position.y - this.floor.offsetY + this.floor.y)
      })
      this.groups.push(this.dragGroup)
    })



    // interaction in the canvas
    const interactObj = interact('.object')
    const self = this
    interactObj.draggable({
      listeners: {
        start(event) {
          console.log(event.type, event.target, event)

        },
        end(event) {
          console.log("on end:", event)

        },
        move(event) {
          console.log('on move:', event)
          if (event.target.getAttribute("id") === "canvas") {
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
            self.groups.forEach((g) => {
              g.shapes.forEach((shape) => {
                shape.x += event.dx
                shape.y += event.dy
              })
            })
          }
          // else {
          //   let i = parseInt(event.target.getAttribute('id'))
          //   console.log(i)
          //   self.objects[i].x += event.dx
          //   self.objects[i].y += event.dy
          // }
        }
      }
    })
  }
}
</script>

<template>
  <div class="office" ref="office">
    <svg class="object" id="canvas" ref="canvas" width="100%" height="100%" xmlns="http://www.w3.org/2000/svg">
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
          <!-- <rect ref="floor" :width="floor.width" :height="floor.height" :x="floor.x" :y="floor.y" fill="url(#grid)" /> -->
          <svg :width="floor.width" :height="floor.height" :x="floor.x" :y="floor.y">
            <rect width="100%" height="100%" fill="url(#grid)" />
          </svg>
        </g>

        <g>
          <g :key="group.i" class="object" v-for="group in groups">
            <rect :key="shape.i" class="object" :x="shape.x" :y="shape.y" :width="shape.width" :height="shape.height"
              :style="shape.style" v-for="shape in group.shapes"></rect>
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
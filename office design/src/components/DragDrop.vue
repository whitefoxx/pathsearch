<script>
export default {
  data() {
    return {
      isDragging: false,
      onDropzone: false,
      dragImage: null,
      targetObject: {
        isMoveable: false,
        toDrop: null
      },
    }
  },
  methods: {
    objectCloneGet(ev) {
      let element = ev.target
      this.isDragging = true
      this.onDropzone = false

      let objectReplica = element.cloneNode(true)
      objectReplica.style.fillOpacity = '0.5'
      objectReplica.id = 'dragImage'
      objectReplica.class = 'dragImage'
      objectReplica.style.position = 'absolute'
      this.$refs['mainSVG1'].appendChild(objectReplica)

      this.dragImage = objectReplica
    },

    objectCloneDragOrigin(ev) {
      this.onDropzone = false
      if (this.isDragging == true && this.dragImage) {
        this.dragImage.setAttribute('x', ev.offsetX)
        this.dragImage.setAttribute('y', ev.offsetY)
      }
    },

    objectCloneDrop() {
      if (this.targetObject.isMoveable && this.targetObject.toDrop) {
        this.targetObject.isMoveable = false;
        this.targetObject.toDrop.style.fillOpacity = '1.0'
        this.targetObject.toDrop = null
      }
      this.isDragging = false
    },

    disableDragOrigin() {
      if (this.isDragging == true && this.dragImage) {
        this.$refs['mainSVG1'].removeChild(this.dragImage)
      }
      this.dragImage = null
      if (!this.onDropzone) {
        this.isDragging = false
      }
    },

    dragOndropzone(ev) {
      this.onDropzone = true
      if (this.isDragging) {
        if (this.targetObject.isMoveable == false && this.dragImage)
          this.enableMove()
        this.disableDragOrigin()
        this.moveObject(ev)
      }
    },

    enableMove() {
      let ele = this.dragImage.cloneNode(true)
      this.targetObject.isMoveable = true
      this.targetObject.toDrop = ele
      this.$refs['mainSVG2'].appendChild(ele)
    },

    moveObject(ev) {
      if (this.targetObject.isMoveable == true && this.targetObject.toDrop) {
        this.targetObject.toDrop.setAttribute('x', ev.offsetX)
        this.targetObject.toDrop.setAttribute('y', ev.offsetY)
      }
    },
  }
}
</script>

<template>
  <svg id="mainSVG1" ref="mainSVG1" @mousemove="objectCloneDragOrigin" @mouseup="disableDragOrigin" width="49.5%">
    <foreignObject width="100%" height="100%">
      <p style="user-select: none; cursor: default">GET ELEMENTS HERE</p>
    </foreignObject>

    <rect id="cloneableObject" height="100" width="100" x="50" y="50" fill="#F8682C" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="50" y="200" fill="#91C300" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="50" y="350" fill="#00B4F1" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="50" y="500" fill="#FFC300" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="200" y="50" fill="#FC736D" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="200" y="200" fill="#BDC0A5" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="200" y="350" fill="#715989" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="200" y="500" fill="#F39B5C" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="350" y="50" fill="#FFA1B1" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="350" y="200" fill="#918ED5" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="350" y="350" fill="#CBBAAA" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="350" y="500" fill="#7698D5" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="500" y="50" fill="#EDE39F" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="500" y="200" fill="#A4A4A4" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="500" y="350" fill="#AC8990" @mousedown="objectCloneGet" />
    <rect id="cloneableObject" height="100" width="100" x="500" y="500" fill="#99FFC4" @mousedown="objectCloneGet" />
  </svg>
  <svg id="mainSVG2" ref="mainSVG2" @mouseup="objectCloneDrop" @mousemove="dragOndropzone" @mouseleave="objectCloneDrop"
    x="49.5%" width="49.5%">
    <foreignObject width="100%" height="100%">
      <p style="user-select: none; cursor: default">
        DROP HERE OR REPOSITION DROPPED ELEMENTS
      </p>
    </foreignObject>
  </svg>
</template>
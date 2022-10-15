<script setup>
import Office from '../components/Office.vue'
import LeftPanel from '../components/LeftPanel.vue'
</script>
    
<template>
    <div class="container">
        <LeftPanel></LeftPanel>
        <Office></Office>
    </div>
</template>
    
<style scoped>
html,
body {
    margin: 0;
    height: 100%;
    overflow: hidden;
}

.container {
    display: flex;
    height: 100%;
}
</style>
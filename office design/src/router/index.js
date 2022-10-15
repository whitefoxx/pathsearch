import { createRouter, createWebHistory } from 'vue-router'
import OfficeDesignView from '../views/OfficeDesignView.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'design',
      component: OfficeDesignView
    },
    {
      path: '/dragdrop',
      name: 'dragdrop',
      // route level code-splitting
      // this generates a separate chunk (About.[hash].js) for this route
      // which is lazy-loaded when the route is visited.
      component: () => import('../views/DragDropView.vue')
    }
  ]
})

export default router

class Node {
  constructor(x, y) {
    this.x = x;
    this.y = y;
    this.parent = null;
    this.pathD = Infinity; // 起点到这个点的实际距离
    this.heuristicD = Infinity; // 这个点到终点的启发函数计算出来的估计距离
    this.key = `${this.x},${this.y}`;
  }

  reset() {
    this.parent = null;
    this.pathD = Infinity;
    this.heuristicD = Infinity;
  }

  isAt(x, y) {
    return this.x === x && this.y === y;
  }

  equal(node) {
    return this.x === node.x && this.y === node.y;
  }

  neighborCoords() {
    const coords = [];
    coords.push([this.x - 1, this.y - 1]);
    coords.push([this.x, this.y - 1]);
    coords.push([this.x + 1, this.y - 1]);
    coords.push([this.x - 1, this.y]);
    coords.push([this.x + 1, this.y]);
    coords.push([this.x - 1, this.y + 1]);
    coords.push([this.x, this.y + 1]);
    coords.push([this.x + 1, this.y + 1]);
    return coords;
  }
}

class NodeSet {
  constructor() {
    this.map = new Map();
  }

  clear() {
    this.map.clear();
  }

  add(node) {
    this.map.set(node.key, node);
  }

  pop() {
    let m = Infinity;
    let node;
    this.map.forEach((n, k) => {
      if (n.pathD + n.heuristicD < m) {
        m = n.pathD + n.heuristicD;
        node = n;
      }
    });
    if (node) this.map.delete(node.key);
    return node;
  }

  has(node) {
    return this.map.has(node.key);
  }

  get(key) {
    return this.map.get(key);
  }

  delete(node) {
    this.map.delete(node.key);
  }
}

function parseSpace(space, fromPosition, toPosition) {
  const rooms = space.rooms;
  let minX = Infinity;
  let minY = Infinity;
  let maxX = -Infinity;
  let maxY = -Infinity;

  rooms.forEach((room) => {
    minX = Math.min(minX, room.x);
    minY = Math.min(minY, room.y);
    maxX = Math.max(maxX, room.x + room.w);
    maxY = Math.max(maxY, room.y + room.h);
  });
  minX = Math.min(minX, fromPosition.x, toPosition.x);
  minY = Math.min(minY, fromPosition.y, toPosition.y);
  maxX = Math.max(maxX, fromPosition.x, toPosition.x);
  maxY = Math.max(maxY, fromPosition.y, toPosition.y);

  maxX += space.unit;
  minX -= space.unit;
  maxY += space.unit;
  minY -= space.unit;

  const columns = Math.ceil((maxX - minX) / space.unit);
  const rows = Math.ceil((maxY - minY) / space.unit);
  const obstacleSet = new NodeSet();
  const originX = Math.floor(minX / space.unit) * space.unit;
  const originY = Math.floor(minY / space.unit) * space.unit;
  rooms.forEach((room) => {
    room.items.forEach((item) => {
      let projections = [];
      if (item.projections === undefined) {
        projections.push({
          x: 0,
          y: 0,
          w: item.w,
          h: item.h,
        });
      } else if (item.projections !== null) {
        projections = item.projections;
      }
      projections.forEach((projection) => {
        const rs = projection.h / space.unit;
        const cs = projection.w / space.unit;
        const offsetC = Math.floor(
          (item.x + projection.x - originX) / space.unit
        );
        const offsetR = Math.floor(
          (item.y + projection.y - originY) / space.unit
        );
        for (let r = 0; r < rs; r++) {
          for (let c = 0; c < cs; c++) {
            const node = new Node(offsetC + c, offsetR + r);
            obstacleSet.add(node);
          }
        }
      });
    });
  });

  return {
    originX,
    originY,
    rows,
    columns,
    obstacleSet,
  };
}

function astarSearch(space, fromPosition, toPosition) {
  const { originX, originY, obstacleSet, rows, columns } = parseSpace(
    space,
    fromPosition,
    toPosition
  );
  const fromX = Math.floor((fromPosition.x - originX) / space.unit);
  const fromY = Math.floor((fromPosition.y - originY) / space.unit);
  const toX = Math.floor((toPosition.x - originX) / space.unit);
  const toY = Math.floor((toPosition.y - originY) / space.unit);
  const fromNode = new Node(fromX, fromY);
  const toNode = new Node(toX, toY);
  const offsetX =
    ((toPosition.x - originX) % space.unit) -
    ((fromPosition.x - originX) % space.unit);
  const offsetY =
    ((toPosition.y - originY) % space.unit) -
    ((fromPosition.y - originY) % space.unit);
  const paths = doAstarSearch(
    space,
    obstacleSet,
    rows,
    columns,
    fromNode,
    toNode,
    offsetX,
    offsetY
  );
  if (paths) {
    let x = fromPosition.x;
    let y = fromPosition.y;
    paths.forEach((s) => {
      x += s[0];
      y += s[1];
    });
    console.assert(
      x === toPosition.x && y === toPosition.y,
      "search path failed!!"
    );
  }
  return paths;
}

function doAstarSearch(
  space,
  obstacleSet,
  rows,
  columns,
  fromNode,
  toNode,
  offsetX,
  offsetY
) {
  const openSet = new NodeSet();
  const closeSet = new NodeSet();

  fromNode.parent = null;
  fromNode.pathD = 0;
  fromNode.heuristicD = heuristic(fromNode, toNode);
  openSet.add(fromNode);

  let realToNode = null; // if the toNode is surrounded by obstacle nodes
  let finalPath = null; // final path to the real toNode

  while (true) {
    let node = openSet.pop();
    if (!node) {
      if (realToNode) return null;

      // find the closest fake toNode which is not a obstacle node
      const m = toNode.y < fromNode.y ? 1 : -1;
      realToNode = toNode;
      let pNode = toNode;
      let y = toNode.y + m;
      while (true) {
        const node = new Node(toNode.x, y);
        pNode.parent = node;
        pNode = node;
        const k = `${toNode.x},${y}`;
        if (!obstacleSet.get(k)) {
          toNode = node;
          finalPath = [0, m * (toNode.y - realToNode.y)];
          break;
        }
        y += m;
      }

      // try again
      openSet.clear();
      closeSet.clear();
      fromNode.parent = null;
      fromNode.pathD = 0;
      fromNode.heuristicD = heuristic(fromNode, toNode);
      openSet.add(fromNode);
      continue;
    }
    closeSet.add(node);
    if (node.equal(toNode)) {
      toNode.parent = node.parent;
      let dx, dy, pRatio, cRatio, pNode;
      const paths = [];
      pNode = null;
      while (node.parent) {
        dy = node.y - node.parent.y;
        dx = node.x - node.parent.x;
        cRatio = dx === 0 ? Infinity : dy / dx;
        if (pNode === null) {
          pRatio = cRatio;
          pNode = node;
        } else if (cRatio !== pRatio) {
          paths.push([
            (pNode.x - node.x) * space.unit,
            (pNode.y - node.y) * space.unit,
          ]);
          pNode = node;
          pRatio = cRatio;
        }
        node = node.parent;
      }
      if (pNode && node && !pNode.equal(node)) {
        paths.push([
          (pNode.x - node.x) * space.unit,
          (pNode.y - node.y) * space.unit,
        ]);
      }

      // add the final path from the fake toNode to the realToNode
      if (realToNode) {
        paths.push(finalPath);
      }

      paths.push([offsetX, offsetY]);
      paths.reverse();
      return paths;
    }
    let stuck = false;
    for (let i = 0; i < 2; i++) {
      let hitN = 0;
      node.neighborCoords().forEach((coord) => {
        const k = `${coord[0]},${coord[1]}`;
        let nb;
        if (
          !stuck &&
          !isOkToMove(obstacleSet, node.x, node.y, coord[0], coord[1])
        ) {
          hitN++;
          // eslint-disable-next-line no-empty
        } else if ((nb = closeSet.get(k))) {
        } else if ((nb = openSet.get(k))) {
          const d = neighborDistance(node, nb);
          if (node.pathD + d < nb.pathD) {
            nb.parent = node;
            nb.pathD = node.pathD + d;
          }
        } else if (
          coord[0] >= 0 &&
          coord[0] < columns &&
          coord[1] < rows &&
          coord[1] >= 0
        ) {
          nb = new Node(coord[0], coord[1]);
          const d = neighborDistance(node, nb);
          nb.pathD = node.pathD + d;
          nb.heuristicD = heuristic(nb, toNode);
          nb.parent = node;
          openSet.add(nb);
        }
      });
      if (hitN < 8) break;
      else stuck = true;
    }
  }
}

function isOkToMove(obstacleSet, x1, y1, x2, y2) {
  const k = `${x2},${y2}`;
  if (obstacleSet.get(k)) return false;
  const d = Math.abs(x1 - x2) + Math.abs(y1 - y2);
  if (d === 1) return true;
  if (d > 2) return false;
  const k1 = `${x1},${y2}`;
  const k2 = `${x2},${y1}`;
  return !obstacleSet.get(k1) && !obstacleSet.get(k2);
}

// 对角线走位的距离，这个值大一点（不能大于2）可能会更走直线一点
const diagonalMoveD = Math.sqrt(3);
function heuristic(from, to) {
  const dx = Math.abs(from.x - to.x);
  const dy = Math.abs(from.y - to.y);
  return dx + dy - (2 - diagonalMoveD) * Math.min(dx, dy);
}

function neighborDistance(a, b) {
  const dx = Math.abs(a.x - b.x);
  const dy = Math.abs(a.y - b.y);
  const d = Math.sqrt(Math.pow(dx, 2) + Math.pow(dy, 2));
  return d;
}

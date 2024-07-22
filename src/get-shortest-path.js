
function parseStdin() {
  let stdin = process.stdin
  let inputChunks = []
  stdin.resume()
  stdin.setEncoding('utf8')
  stdin.on('data', function (chunk) {
    inputChunks.push(chunk);
  });
  return new Promise((resolve, reject) => {
    stdin.on('end', function () {
      let inputJSON = inputChunks.join('')
      resolve(JSON.parse(inputJSON))
    })
    stdin.on('error', function () {
      reject(Error('error during read'))
    })
    stdin.on('timeout', function () {
      reject(Error('timeout during read'))
    })
  })
}

function searchForPath(minSpanTree, source, destination) {
  if (source === destination) {
    return [destination];
  }
  for (const adjacentVertex of minSpanTree[source]) {
    const path = searchForPath(minSpanTree, adjacentVertex, destination);
    if (path) {
      path.unshift(source);
      return path;
    }
  }
  return null;
}

async function main() {
  const [,, source, destination] = process.argv;
  if (!(source && destination)) {
    console.error(`Arguments are not formed correctly. \nExpected: "source destination" \nReceived: ${source} ${destination}`);
    process.exit(1);
  }
  const allMinSpanTrees = await parseStdin();
  const minSpanTree = allMinSpanTrees[source];
  const path = searchForPath(minSpanTree, source, destination);
  console.log(path || '"No path found."');
}

main()
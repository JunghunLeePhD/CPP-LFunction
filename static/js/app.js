let chartLog = null;
let chartCLT = null;

// 1. HELPER: GCD
function gcd(a, b) {
    while (b) {
        [a, b] = [b, a % b];
    }
    return a;
}

// 2. MAIN LOGIC: Handle Modulus Change
function onModulusChange() {
    const qInput = document.getElementById('mod');
    const kSelect = document.getElementById('char-select');

    let q = parseInt(qInput.value);
    if (isNaN(q) || q < 1) {
        q = 1;
        qInput.value = 1;
    }

    const currentK = parseInt(kSelect.value) || 1;
    kSelect.innerHTML = '';

    let charIndex = 1;

    for (let k = 1; k <= q; k++) {
        if (gcd(k, q) === 1) {
            const option = document.createElement('option');

            option.value = charIndex;

            if (k === 1) option.text = `${k} (Principal)`;
            else if (k === q - 1 && q > 2) option.text = `${k}`;
            else option.text = k;

            kSelect.appendChild(option);

            charIndex++;
        }
    }

    let exists = false;
    for (let opt of kSelect.options) {
        if (parseInt(opt.value) === currentK) {
            kSelect.value = currentK;
            exists = true;
            break;
        }
    }
    if (!exists) kSelect.value = 1;

    updateCharts();
}

const commonOptions = {
    responsive: true,
    maintainAspectRatio: false,
    interaction: { mode: 'index', intersect: false },
    scales: { x: { title: { display: true, text: 't' } } },
    animation: false,
    onClick: (e) => {
        const chart = e.chart;
        const points = chart.getElementsAtEventForMode(e, 'nearest', { intersect: false }, true);
        if (points.length) {
            const xValue = chart.scales.x.getValueForPixel(e.x);
            zoomTo(xValue);
        }
    },
};

async function updateCharts() {
    const r = document.getElementById('real').value;
    const start = document.getElementById('start').value;
    const end = document.getElementById('end').value;
    const q = document.getElementById('mod').value;
    const idx = document.getElementById('char-select').value || 1;
    const steps = Math.min((end - start) * 10, 100000);

    const loader = document.getElementById('loading-sign');
    if (loader) loader.style.display = 'block';

    try {
        const response = await fetch(
            `/scan_flint?r=${r}&start=${start}&end=${end}&q=${q}&idx=${idx}&steps=${steps}`
        );
        const data = await response.json();

        if (data.error) {
            console.error(data.error);
            return;
        }

        const points = data.points;
        const labels = points.map((p) => p.t.toFixed(4));
        const realData = points.map((p) => p.real);

        if (chartLog) chartLog.destroy();
        chartLog = new Chart(document.getElementById('logChart'), {
            type: 'line',
            data: {
                labels: labels,
                datasets: [
                    {
                        label: 'log|L(0.5+it)|',
                        data: realData,
                        borderColor: 'rgb(255, 99, 132)',
                        borderWidth: 2,
                        pointRadius: 0,
                    },
                ],
            },
            options: commonOptions,
        });
    } catch (error) {
        console.error('Error computing:', error);
    } finally {
        if (loader) loader.style.display = 'none';
    }
}

// 4. INITIALIZATION
document.addEventListener('DOMContentLoaded', onModulusChange);

// 5. KEYBOARD HANDLERS
document.addEventListener('keydown', function (event) {
    const modInput = document.getElementById('mod');
    const endInput = document.getElementById('end');
    const charInput = document.getElementById('char-select');

    const active = document.activeElement;
    if (active.tagName === 'INPUT' && active !== modInput && active !== endInput) {
        return;
    }
    if (event.shiftKey) {
        if (event.key === 'Escape') {
            resetZoom();
            event.preventDefault();
        }

        if (event.key === 'ArrowUp') {
            modInput.stepUp();
            onModulusChange();
            event.preventDefault();
        }
        if (event.key === 'ArrowDown') {
            modInput.stepDown();
            onModulusChange();
            event.preventDefault();
        }
        if (event.key === 'ArrowRight') {
            endInput.stepUp();
            updateCharts();
            event.preventDefault();
        }
        if (event.key === 'ArrowLeft') {
            endInput.stepDown();
            updateCharts();
            event.preventDefault();
        }
    }

    if (event.altKey) {
        if (event.key === 'ArrowDown') {
            if (charInput.selectedIndex > 0) {
                charInput.selectedIndex--;
                updateCharts();
            }
            event.preventDefault();
        }
        if (event.key === 'ArrowUp') {
            if (charInput.selectedIndex < charInput.options.length - 1) {
                charInput.selectedIndex++;
                updateCharts();
            }
            event.preventDefault();
        }
    }
});

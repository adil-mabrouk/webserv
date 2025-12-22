#!/usr/bin/php-cgi
<?php
header("Content-Type: text/html; charset=UTF-8");
header("X-Powered-By: WebServ-CGI/1.0");
header("X-Test-Header: CustomValue");
sleep(5);
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WebServ CGI Test Suite</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            padding: 20px;
            line-height: 1.6;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            border-radius: 10px;
            box-shadow: 0 10px 40px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        
        header {
            background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }
        
        header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
        }
        
        header p {
            font-size: 1.1em;
            opacity: 0.9;
        }
        
        .section {
            padding: 30px;
            border-bottom: 1px solid #e0e0e0;
        }
        
        .section:last-child {
            border-bottom: none;
        }
        
        .section h2 {
            color: #2a5298;
            margin-bottom: 20px;
            padding-bottom: 10px;
            border-bottom: 3px solid #667eea;
            display: inline-block;
        }
        
        table {
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
            background: white;
        }
        
        table thead {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }
        
        table th {
            padding: 15px;
            text-align: left;
            font-weight: 600;
            text-transform: uppercase;
            font-size: 0.9em;
            letter-spacing: 1px;
        }
        
        table td {
            padding: 12px 15px;
            border-bottom: 1px solid #e0e0e0;
        }
        
        table tbody tr:hover {
            background: #f5f5f5;
            transition: background 0.3s;
        }
        
        table tbody tr:nth-child(even) {
            background: #fafafa;
        }
        
        .badge {
            display: inline-block;
            padding: 5px 12px;
            border-radius: 20px;
            font-size: 0.85em;
            font-weight: 600;
            margin: 2px;
        }
        
        .badge-success {
            background: #4caf50;
            color: white;
        }
        
        .badge-info {
            background: #2196f3;
            color: white;
        }
        
        .badge-warning {
            background: #ff9800;
            color: white;
        }
        
        .badge-danger {
            background: #f44336;
            color: white;
        }
        
        .info-box {
            background: #e3f2fd;
            border-left: 4px solid #2196f3;
            padding: 15px;
            margin: 15px 0;
            border-radius: 4px;
        }
        
        .success-box {
            background: #e8f5e9;
            border-left: 4px solid #4caf50;
            padding: 15px;
            margin: 15px 0;
            border-radius: 4px;
        }
        
        .code-block {
            background: #263238;
            color: #aed581;
            padding: 20px;
            border-radius: 5px;
            overflow-x: auto;
            margin: 15px 0;
            font-family: 'Courier New', monospace;
            font-size: 0.9em;
            line-height: 1.5;
        }
        
        .grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin: 20px 0;
        }
        
        .card {
            background: white;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            padding: 20px;
            text-align: center;
            transition: transform 0.3s, box-shadow 0.3s;
        }
        
        .card:hover {
            transform: translateY(-5px);
            box-shadow: 0 5px 20px rgba(0,0,0,0.1);
        }
        
        .card h3 {
            color: #2a5298;
            margin-bottom: 10px;
        }
        
        .card p {
            color: #666;
            font-size: 2em;
            font-weight: bold;
            color: #667eea;
        }
        
        footer {
            background: #263238;
            color: white;
            text-align: center;
            padding: 20px;
            font-size: 0.9em;
        }
        
        .timestamp {
            color: #aaa;
            font-size: 0.9em;
            font-style: italic;
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>🚀 WebServ CGI Test Suite</h1>
            <p>Comprehensive CGI Environment & Request Testing</p>
            <p class="timestamp">Generated: <?php echo date('Y-m-d H:i:s'); ?></p>
        </header>

        <!-- REQUEST INFORMATION -->
        <div class="section">
            <h2>📋 Request Information</h2>
            
            <div class="grid">
                <div class="card">
                    <h3>Method</h3>
                    <p><?php echo htmlspecialchars($_SERVER['REQUEST_METHOD'] ?? 'N/A'); ?></p>
                </div>
                <div class="card">
                    <h3>Protocol</h3>
                    <p><?php echo htmlspecialchars($_SERVER['SERVER_PROTOCOL'] ?? 'N/A'); ?></p>
                </div>
                <div class="card">
                    <h3>Gateway</h3>
                    <p><?php echo htmlspecialchars($_SERVER['GATEWAY_INTERFACE'] ?? 'N/A'); ?></p>
                </div>
                <div class="card">
                    <h3>Content Length</h3>
                    <p><?php echo htmlspecialchars($_SERVER['CONTENT_LENGTH'] ?? '0'); ?></p>
                </div>
            </div>

            <div class="info-box">
                <strong>Script:</strong> <?php echo htmlspecialchars($_SERVER['SCRIPT_FILENAME'] ?? 'N/A'); ?><br>
                <strong>Query String:</strong> <?php echo htmlspecialchars($_SERVER['QUERY_STRING'] ?? '(empty)'); ?><br>
                <strong>Request URI:</strong> <?php echo htmlspecialchars($_SERVER['REQUEST_URI'] ?? 'N/A'); ?>
            </div>
        </div>

        <!-- HTTP HEADERS -->
        <div class="section">
            <h2>📨 HTTP Headers (Environment Variables)</h2>
            
            <?php
            $http_headers = array();
            foreach ($_SERVER as $key => $value) {
                if (strpos($key, 'HTTP_') === 0) {
                    $http_headers[$key] = $value;
                }
            }
            ksort($http_headers);
            ?>
            
            <div class="success-box">
                Found <strong><?php echo count($http_headers); ?></strong> HTTP header(s)
            </div>
            
            <?php if (!empty($http_headers)): ?>
            <table>
                <thead>
                    <tr>
                        <th style="width: 40%">Environment Variable</th>
                        <th style="width: 60%">Value</th>
                    </tr>
                </thead>
                <tbody>
                <?php foreach ($http_headers as $key => $value): ?>
                    <tr>
                        <td><span class="badge badge-info"><?php echo htmlspecialchars($key); ?></span></td>
                        <td><?php echo htmlspecialchars($value); ?></td>
                    </tr>
                <?php endforeach; ?>
                </tbody>
            </table>
            <?php else: ?>
            <div class="info-box">No HTTP headers found in environment.</div>
            <?php endif; ?>
        </div>

        <!-- CONTENT HEADERS -->
        <div class="section">
            <h2>📄 Content Headers</h2>
            
            <table>
                <thead>
                    <tr>
                        <th>Variable</th>
                        <th>Value</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td><span class="badge badge-warning">CONTENT_TYPE</span></td>
                        <td><?php echo htmlspecialchars($_SERVER['CONTENT_TYPE'] ?? '(not set)'); ?></td>
                    </tr>
                    <tr>
                        <td><span class="badge badge-warning">CONTENT_LENGTH</span></td>
                        <td><?php echo htmlspecialchars($_SERVER['CONTENT_LENGTH'] ?? '0'); ?></td>
                    </tr>
                </tbody>
            </table>
        </div>

        <!-- REQUEST BODY (POST/PUT) -->
        <div class="section">
            <h2>📝 Request Body (stdin)</h2>
            
            <?php
            $input = file_get_contents('php://stdin');
            $input_length = strlen($input);
            ?>
            
            <div class="info-box">
                <strong>Body Length:</strong> <?php echo $input_length; ?> bytes<br>
                <strong>Expected Length (CONTENT_LENGTH):</strong> <?php echo $_SERVER['CONTENT_LENGTH'] ?? '0'; ?> bytes
            </div>
            
            <?php if ($input_length > 0): ?>
            <div class="success-box">
                ✓ Request body received successfully
            </div>
            <div class="code-block"><?php echo htmlspecialchars($input); ?></div>
            
            <?php
            // Try to parse as form data
            if (isset($_SERVER['CONTENT_TYPE']) && 
                strpos($_SERVER['CONTENT_TYPE'], 'application/x-www-form-urlencoded') !== false) {
                
                parse_str($input, $parsed_data);
                
                if (!empty($parsed_data)) {
                    echo '<h3 style="margin-top: 20px;">Parsed Form Data:</h3>';
                    echo '<table><thead><tr><th>Key</th><th>Value</th></tr></thead><tbody>';
                    
                    foreach ($parsed_data as $key => $value) {
                        echo '<tr>';
                        echo '<td><span class="badge badge-success">' . htmlspecialchars($key) . '</span></td>';
                        echo '<td>' . htmlspecialchars($value) . '</td>';
                        echo '</tr>';
                    }
                    
                    echo '</tbody></table>';
                }
            }
            
            // Try to parse as JSON
            if (isset($_SERVER['CONTENT_TYPE']) && 
                strpos($_SERVER['CONTENT_TYPE'], 'application/json') !== false) {
                
                $json_data = json_decode($input, true);
                
                if ($json_data !== null) {
                    echo '<h3 style="margin-top: 20px;">Parsed JSON Data:</h3>';
                    echo '<div class="code-block">' . htmlspecialchars(json_encode($json_data, JSON_PRETTY_PRINT)) . '</div>';
                }
            }
            ?>
            
            <?php else: ?>
            <div class="info-box">
                No request body (this is normal for GET requests)
            </div>
            <?php endif; ?>
        </div>

        <!-- QUERY STRING PARAMETERS -->
        <div class="section">
            <h2>🔍 Query String Parameters</h2>
            
            <?php
            $query_string = $_SERVER['QUERY_STRING'] ?? '';
            parse_str($query_string, $query_params);
            ?>
            
            <div class="info-box">
                <strong>Raw Query String:</strong> <?php echo htmlspecialchars($query_string ?: '(empty)'); ?>
            </div>
            
            <?php if (!empty($query_params)): ?>
            <table>
                <thead>
                    <tr>
                        <th>Parameter</th>
                        <th>Value</th>
                    </tr>
                </thead>
                <tbody>
                <?php foreach ($query_params as $key => $value): ?>
                    <tr>
                        <td><span class="badge badge-success"><?php echo htmlspecialchars($key); ?></span></td>
                        <td><?php echo htmlspecialchars($value); ?></td>
                    </tr>
                <?php endforeach; ?>
                </tbody>
            </table>
            <?php else: ?>
            <div class="info-box">No query parameters found.</div>
            <?php endif; ?>
        </div>

        <!-- ALL CGI ENVIRONMENT VARIABLES -->
        <div class="section">
            <h2>🌍 All CGI Environment Variables</h2>
            
            <?php
            $all_vars = $_SERVER;
            ksort($all_vars);
            ?>
            
            <div class="success-box">
                Total environment variables: <strong><?php echo count($all_vars); ?></strong>
            </div>
            
            <table>
                <thead>
                    <tr>
                        <th style="width: 35%">Variable Name</th>
                        <th style="width: 65%">Value</th>
                    </tr>
                </thead>
                <tbody>
                <?php foreach ($all_vars as $key => $value): ?>
                    <tr>
                        <td>
                            <?php
                            if (strpos($key, 'HTTP_') === 0) {
                                echo '<span class="badge badge-info">' . htmlspecialchars($key) . '</span>';
                            } elseif (strpos($key, 'CONTENT_') === 0) {
                                echo '<span class="badge badge-warning">' . htmlspecialchars($key) . '</span>';
                            } elseif (in_array($key, ['REQUEST_METHOD', 'SCRIPT_FILENAME', 'QUERY_STRING', 'SERVER_PROTOCOL'])) {
                                echo '<span class="badge badge-success">' . htmlspecialchars($key) . '</span>';
                            } else {
                                echo '<span class="badge badge-danger">' . htmlspecialchars($key) . '</span>';
                            }
                            ?>
                        </td>
                        <td style="word-break: break-all;"><?php echo htmlspecialchars($value); ?></td>
                    </tr>
                <?php endforeach; ?>
                </tbody>
            </table>
        </div>

        <!-- SERVER CAPABILITIES -->
        <div class="section">
            <h2>⚙️ Server Capabilities</h2>
            
            <div class="grid">
                <div class="card">
                    <h3>PHP Version</h3>
                    <p style="font-size: 1.5em;"><?php echo PHP_VERSION; ?></p>
                </div>
                <div class="card">
                    <h3>Server Time</h3>
                    <p style="font-size: 1.2em;"><?php echo date('H:i:s'); ?></p>
                </div>
                <div class="card">
                    <h3>Server Date</h3>
                    <p style="font-size: 1.2em;"><?php echo date('Y-m-d'); ?></p>
                </div>
                <div class="card">
                    <h3>Timezone</h3>
                    <p style="font-size: 1.2em;"><?php echo date_default_timezone_get(); ?></p>
                </div>
            </div>
        </div>

        <!-- TEST INSTRUCTIONS -->
        <div class="section">
            <h2>🧪 Test Instructions</h2>
            
            <h3 style="margin-top: 20px; color: #2a5298;">Test with GET:</h3>
            <div class="code-block">curl "http://localhost:8080/cgi-bin/test.php?name=John&age=25&city=Paris"</div>
            
            <h3 style="margin-top: 20px; color: #2a5298;">Test with POST (form data):</h3>
            <div class="code-block">curl -X POST http://localhost:8080/cgi-bin/test.php \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "username=john&password=secret&email=john@example.com"</div>
            
            <h3 style="margin-top: 20px; color: #2a5298;">Test with POST (JSON):</h3>
            <div class="code-block">curl -X POST http://localhost:8080/cgi-bin/test.php \
  -H "Content-Type: application/json" \
  -d '{"user":"john","action":"login","timestamp":1234567890}'</div>
            
            <h3 style="margin-top: 20px; color: #2a5298;">Test with custom headers:</h3>
            <div class="code-block">curl http://localhost:8080/cgi-bin/test.php \
  -H "Authorization: Bearer token123" \
  -H "X-Custom-Header: CustomValue" \
  -H "X-API-Key: secret_key_here"</div>
        </div>

        <footer>
            <p><strong>WebServ CGI Test Suite</strong> | Powered by PHP <?php echo PHP_VERSION; ?></p>
            <p style="margin-top: 10px; opacity: 0.7;">Test your CGI implementation thoroughly!</p>
        </footer>
    </div>
</body>
</html>